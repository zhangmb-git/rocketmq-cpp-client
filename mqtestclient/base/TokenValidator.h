/*
 * TokenValidator.h
 *
 *  Created on: 2013-10-2
 *      Author: ziteng@mogujie.com
 */

#ifndef TOKENVALIDATOR_H_
#define TOKENVALIDATOR_H_

#include "util.h"

/**
 * return 0 if generate token successful
 */
int genToken(uint64_t uid, time_t time_offset, char* md5_str_buf);

bool IsTokenValid(uint64_t user_id, const char* token);


#endif /* TOKENVALIDATOR_H_ */
