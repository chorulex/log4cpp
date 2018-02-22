/**
 * Light weight log lib for c++.
 * 
 * log_level.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_LEVEL_H_
#define _LOG4CPP_LEVEL_H_

namespace Log4CPP
{
enum class Level : int
{
    ALL = 0,    // open all level.
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF         // turn off all level.
};
}

#endif