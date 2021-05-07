#ifndef _SLSClient_INCLUDE_
#define _SLSClient_INCLUDE_

#include <list>

#include "SLSRelay.hpp"
#include "TSFileTimeReader.hpp"


/**
 * CSLSClient
 */
class CSLSClient: public CSLSRelay
{
public :
	CSLSClient();
    virtual ~CSLSClient();

    int play(const char *url, const char *out_file_name);
    int push(const char *url, const char *ts_file_name, bool loop);

    virtual int close();
    virtual int handler();

    int64_t get_bitrate();

protected:
    int init_epoll();
    int uninit_epoll();

    int open_url(const char* url);
    int write_data_handler();
    int read_data_handler();


    char  m_url[1024];
    char  m_ts_file_name[1024];
    char  m_out_file_name[1024];

    int   m_eid;
    int   m_out_file;

    int64_t m_data_count;
    int64_t m_bit_rate;//kbit/s

    CTSFileTimeReader   *m_ts_file_time_reader;
    CSLSSyncClock        m_sync_clock;

};


#endif
