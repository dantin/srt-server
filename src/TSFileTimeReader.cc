
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Edward.Wu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <sys/stat.h>
#include <sys/types.h>
#include <cstdarg>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "TSFileTimeReader.hpp"
#include "SLSLog.hpp"

#define RTS_PACK_LEN (1316 + sizeof(int64_t))
#define RTS_BUF_SIZE (RTS_PACK_LEN * 100)


CTSFileTimeReader::CTSFileTimeReader()
{
	m_rts_fd       = 0;
	m_loop         = true;
	m_dts_pid      = INVALID_PID;
    m_dts          = INVALID_DTS_PTS;
    m_pts          = INVALID_DTS_PTS;
    m_readed_count = 0;
    m_udp_duration = 0;

    memset(m_file_name, 0, URL_MAX_LEN);
}

CTSFileTimeReader::~CTSFileTimeReader()
{
}

int CTSFileTimeReader::open(const char *ts_file_name, bool loop)
{
    if (NULL == ts_file_name || strlen(ts_file_name) == 0) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::open, wrong file_name='%s'.",
                this, m_file_name);
    	return SLS_ERROR;
    }

    int ret = generate_rts_file(ts_file_name);
    if (SLS_OK != ret) {
    	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::open， generate_rts_file failed, '%s'.\n",
    			this, ts_file_name);
    }

    m_rts_fd = ::open(m_file_name, O_RDONLY);
    if (m_rts_fd <= 0) {
    	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::open, open file='%s' failed, '%s'.\n",
    			this, ts_file_name, strerror(errno));
    	return ret;
    }
    m_array_data.setSize(RTS_BUF_SIZE);

    m_loop = loop;
    m_readed_count = 0;
	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::open, ok, fd=%d, file_name='%s', loop=%d.\n",
			this, m_rts_fd, m_file_name, m_loop);
	return SLS_OK;
}

int CTSFileTimeReader::close()
{
    if (m_rts_fd > 0) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::close, ok, fd=%d, file_name='%s'.",
                this, m_rts_fd, m_file_name);
    	::close(m_rts_fd);
        m_rts_fd = 0;
    }
    return SLS_OK;
}

int  CTSFileTimeReader::get(uint8_t *data, int size, int64_t &tm_ms, bool& jitter)
{
	jitter = false;
    if (m_rts_fd <= 0) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, failed, fd=%d, file_name='%s'.",
                this, m_rts_fd, m_file_name);
    	return SLS_ERROR;
    }

    if (m_array_data.count() == 0) {
    	uint8_t rts_data[RTS_BUF_SIZE];
    	int n = ::read(m_rts_fd, rts_data, RTS_BUF_SIZE);
    	if (n > 0){
    		m_array_data.put(rts_data, n);
    	} else {
        	if (!m_loop) {
    	    	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, file end, file='%s'.\n",
    	    			this, m_file_name);
    	    	return SLS_ERROR;
        	}
			sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, loop=1, reopen file='%s'.\n",
					this, m_file_name);
			//1. read data from header again.
			::close(m_rts_fd);
			m_rts_fd = ::open(m_file_name, O_RDONLY);
			if (0 == m_rts_fd) {
				sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, open file='%s' failed, '%s'.\n",
						this, m_file_name, strerror(errno));
				return SLS_OK;
			}
			m_readed_count = 0;
			n = ::read(m_rts_fd, rts_data, RTS_BUF_SIZE);
			if (n > 0){
				m_array_data.put(data, n);
			} else {
	        	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, read data failed, '%s'.\n",
	        			this, strerror(errno));
				return SLS_ERROR;
			}
    	}
    }

    if (m_array_data.count() == 0) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, failed, m_array.count() is zero, file_name='%s'.",
                this, m_file_name);
    	return SLS_ERROR;
    }

    int64_t rts = INVALID_DTS_PTS;
    int ret = m_array_data.get((uint8_t*)&rts, sizeof(rts));
    if (ret != sizeof(rts)) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, failed, m_array_data.get rts, ret=%d.",
                this, m_file_name, ret);
    	return SLS_ERROR;
    }
    tm_ms = rts/90;//rts is 90K clock

    ret = m_array_data.get(data, size);
    if (ret > 0) {
    	m_readed_count += ret;
    }
    if (ret != size) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::get, failed, m_array_data. get data, ret=%d, not %d.",
                this, ret, m_file_name, ret, size);
    	return SLS_ERROR;
    }
    return SLS_OK;
}

int64_t CTSFileTimeReader::generate_rts_file(const char  *ts_file_name)
{
    int ret = SLS_ERROR;
    if (NULL == ts_file_name && strlen(ts_file_name) == 0) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, failed, ts_file_name='%s'.",
                this, ts_file_name);
    	return ret;
    }
    char rts_file_name[URL_MAX_LEN] = {0};
    sprintf(rts_file_name, "%s.rts", ts_file_name);
    strcpy(m_file_name, rts_file_name);

    struct stat rts_file;
    if (0 == stat(rts_file_name, &rts_file)) {
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, ts_file_name='%s' exist.",
                this, rts_file_name);
        return SLS_OK;
    }

    int ts_fd = ::open(ts_file_name, O_RDONLY);
    if (ts_fd <= 0) {
    	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, open file='%s' failed, '%s'.\n",
    			this, ts_file_name, strerror(errno));
    	return ret;
    }

    m_rts_fd = ::open(rts_file_name, O_WRONLY|O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IXOTH);
    if (m_rts_fd <= 0) {
    	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, create file='%s' failed, '%s'.\n",
    			this, rts_file_name, strerror(errno));
    	if (ts_fd)
    	    ::close(ts_fd);
    	return ret;
    }

    m_array_data.setSize(TS_UDP_LEN * 1000);
    uint8_t ts_pack[TS_PACK_LEN] = {0};
    int ts_index = 0;
    int dts_index = 0;
    ts_info ti;
    sls_init_ts_info(&ti);
    while(true) {
    	int n = read(ts_fd, ts_pack, TS_PACK_LEN);
    	if (n < TS_PACK_LEN)
    		break;
    	sls_parse_ts_info((const uint8_t *)ts_pack, &ti);
        if (INVALID_DTS_PTS == ti.dts ) {
            m_array_data.put(ts_pack, TS_PACK_LEN);
            ts_index ++;
            continue;
        }
        if (INVALID_DTS_PTS == m_dts) {
            //the 1st dts
            sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, ts_index=%d, dts_index=%d, ti.es_pid=%d, dts=%lld.",
                    this, ts_index, dts_index, ti.es_pid, ti.dts);
            m_dts = ti.dts;
            m_pts = ti.pts;
            ti.dts = INVALID_DTS_PTS;
            ti.pts = INVALID_DTS_PTS;
            m_dts_pid = ti.es_pid;

            m_array_data.put(ts_pack, TS_PACK_LEN);
            ts_index ++;
            dts_index ++;
            continue;
        }
        //write the rts packet
        int ts_count = m_array_data.count();
        int ts_udp_count = ts_count/TS_UDP_LEN;
        if (ts_udp_count > 0) {
            m_udp_duration = (ti.dts - m_dts)/ts_udp_count;
            int64_t rts = m_dts;
            uint8_t rts_data[TS_UDP_LEN];
            uint8_t p[8] = {0};
            for (int i = 0; i < ts_udp_count; i++) {
                int n = m_array_data.get(rts_data, TS_UDP_LEN);
                if (n != TS_UDP_LEN) {
                    printf("ts2es: m_array_data.get, wrong n=%d, not %d.\n", n, TS_UDP_LEN);
                    break;
                }
                write(m_rts_fd, &rts, sizeof(rts));
                write(m_rts_fd, rts_data, TS_UDP_LEN);
                rts += m_udp_duration;
            }
            m_dts = ti.dts;
            m_pts = ti.pts;
        }
        sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, ts_index=%d, dts_index=%d, m_dts_pid=%d, dts=%lld.",
                this, ts_index, dts_index, m_dts_pid, ti.dts);
        if (ti.sps_len > 0) {
            sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, ts_index=%d, dts_index=%d, m_dts_pid=%d, sps_len=%d, pps_len=%d.",
                    this, ts_index, dts_index, m_dts_pid, ti.sps_len, ti.pps_len);
        }
        ti.dts = INVALID_DTS_PTS;
        ti.pts = INVALID_DTS_PTS;

        m_array_data.put(ts_pack, TS_PACK_LEN);
        ts_index ++;
        dts_index ++;
    }
    //last data in m_array_data
	int64_t rts = m_dts;
	int ts_count = m_array_data.count();
	int ts_udp_count = ts_count/TS_UDP_LEN;
	int n = 0;
	uint8_t udp_data[TS_UDP_LEN];
	if (ts_udp_count > 0){
		for (int i = 0; i < ts_udp_count+1; i++) {
			n = m_array_data.get(udp_data, TS_UDP_LEN);
			if (n != TS_UDP_LEN) {
				break;
			}
			write(m_rts_fd, &rts, sizeof(rts));
			write(m_rts_fd, udp_data, TS_UDP_LEN);
			rts += m_udp_duration;
		}
	}
	//remainder
	if (n > 0) {
		//fill the remainder with null packet
		for (int i = n; i < TS_UDP_LEN;) {
		    udp_data[i] = 0x47;
		    udp_data[i+1] = 0x1F;
		    udp_data[i+2] = 0xFF;
			i += TS_PACK_LEN;
		}
		write(m_rts_fd, &rts, sizeof(rts));
		write(m_rts_fd, udp_data, TS_UDP_LEN);
	}

    ::close(ts_fd);
    ::close(m_rts_fd);
	sls_log(SLS_LOG_INFO, "[%p]CTSFileTimeReader::generate_rts_file, ok, file='%s'.\n",
			this, rts_file_name);

    return SLS_OK;
}



