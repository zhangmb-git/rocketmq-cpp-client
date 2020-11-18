#ifndef __UTIL_H__
#define __UTIL_H__

#define _CRT_SECURE_NO_DEPRECATE	// remove warning C4996, 

#include "ostype.h"
#include "UtilPdu.h"
#include "Lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slog/slog_api.h"
#ifndef _WIN32
#include <strings.h>
#endif

#include <sys/stat.h>
#include <assert.h>
#include <vector>

#ifdef _WIN32
#define	snprintf	sprintf_s
#else
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#endif

#define NOTUSED_ARG(v) ((void)v)		// used this to remove warning C4100, unreferenced parameter

/// yunfan modify end 
class CRefObject
{
public:
	CRefObject();
	virtual ~CRefObject();

	void SetLock(CLock* lock) { m_lock = lock; }
	void AddRef();
	void ReleaseRef();
private:
	int				m_refCount;
	CLock*	m_lock;
};

#define LOG_MODULE_IM         "IM"


extern CSLog g_imlog;

// Add By ZhangYuanhao 2015-01-14 For log show the file name not the full path + filename
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#if defined(_WIN32) || defined(_WIN64)
#define log(fmt, ...)  g_imlog.Info("<%s>\t<%d>\t<%s>,"fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define log(fmt, args...)  g_imlog.Info("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define loge(fmt, args...)  g_imlog.Error("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define WARN(fmt, args...)  g_imlog.Warn("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define DEBUG(fmt, args...)  g_imlog.Debug("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define ERROR(fmt, args...)  g_imlog.Error("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#define FATAL(fmt, args...)  g_imlog.Fatal("<%s>|<%d>|<%s>," fmt, __FILENAME__, __LINE__, __FUNCTION__, ##args)
#endif
//#define log(fmt, ...)  g_imlog.Info("<%s>\t<%d>\t<%s>,"+fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

uint64_t get_tick_count();
void util_sleep(uint32_t millisecond);


class CStrExplode
{
public:
	CStrExplode(char* str, char seperator);
	virtual ~CStrExplode();

	uint32_t GetItemCnt() { return m_item_cnt; }
	char* GetItem(uint32_t idx) { return m_item_list[idx]; }
private:
	uint32_t	m_item_cnt;
	char** 		m_item_list;
};

char* replaceStr(char* pSrc, char oldChar, char newChar);
string int2string(uint64_t user_id);
uint32_t string2int(const string& value);
uint64_t string2long(const string& value);
void replace_mark(string& str, string& new_value, uint32_t& begin_pos);
void replace_mark(string& str, uint32_t new_value, uint32_t& begin_pos);
void replace_all(string &str, const string &old_value, const string &new_value);
/**
 * 分隔字符串
 string s = "a,b*c*d,e";
    vector<string> v;
    split_string(s, v, "*"); //a,b c d,e
    for (vector<string>::size_type i = 0; i != v.size(); ++i)
        cout << v[i] << " ";
    cout << endl;
 * @param source:源字符串
 * @param outStrings:输出
 * @param split:分隔符
 */
void split_string(const std::string &source, std::vector<std::string> &outStrings, const std::string &split);

// 去除空字符串
string& ltrim(string &str);
string& rtrim(string &str);
string& trim(string &str);

void writePid();
inline unsigned char toHex(const unsigned char &x);
inline unsigned char fromHex(const unsigned char &x);
string URLEncode(const string &sIn);
string URLDecode(const string &sIn);


int64_t get_file_size(const char *path);
const char*  memfind(const char *src_str,size_t src_len, const char *sub_str, size_t sub_len, bool flag = true);


#endif
