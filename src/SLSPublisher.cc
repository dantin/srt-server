#include <errno.h>
#include <string.h>


#include "SLSPublisher.hpp"
#include "SLSPlayer.hpp"
#include "SLSLog.hpp"

/**
 * app conf
 */
SLS_CONF_DYNAMIC_IMPLEMENT(app)

/**
 * CSLSPublisher class implementation
 */

CSLSPublisher::CSLSPublisher()
{
    m_is_write             = 0;
    m_map_publisher        = NULL;

    sprintf(m_role_name, "publisher");

}

CSLSPublisher::~CSLSPublisher()
{
    //release
}

int CSLSPublisher::init()
{
    int ret = CSLSRole::init();
    if (m_conf) {
        sls_conf_app_t * app_conf = ((sls_conf_app_t *)m_conf);
        //m_exit_delay = ((sls_conf_app_t *)m_conf)->publisher_exit_delay;
        strcpy(m_record_hls, app_conf->record_hls);
        m_record_hls_segment_duration = app_conf->record_hls_segment_duration;
    }

    return ret;
}

int CSLSPublisher::uninit()
{
    int ret = SLS_OK;

	if (m_map_data) {
        ret = m_map_data->remove(m_map_data_key);
		sls_log(SLS_LOG_INFO, "[%p]CSLSPublisher::uninit, removed publisher from m_map_data, ret=%d.",
				this, ret);
	}

	if (m_map_publisher) {
        ret = m_map_publisher->remove(this);
		sls_log(SLS_LOG_INFO, "[%p]CSLSPublisher::uninit, removed publisher from m_map_publisher, ret=%d.",
				this, ret);
	}
    return CSLSRole::uninit();
}

void CSLSPublisher::set_map_publisher(CSLSMapPublisher * publisher)
{
	m_map_publisher = publisher;
}

int CSLSPublisher::handler()
{
    return handler_read_data();
}



