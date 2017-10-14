#pragma once
#include <curl/curl.h>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <string>
#include <gtkmm.h>
#include <curl/curl.h>
#include <iostream>
#include <sys/stat.h>
#include "appWin.h"
class appWin;

class Connection
{
	private:
		pthread_t id;
		std::string name;
		std::string url;
		FILE* fp;
		off_t start_from;
		int iCountBadPerform;
		struct stat statbuf;
		bool downloaded;
		

	public:
		//variables
		std::string status;
		Glib::Dispatcher dispatcher;
		double progress;
		bool paused;
		std::mutex c_mutex;
		std::mutex c_variable_mutex;
		std::condition_variable c_variable;		
		appWin* App;
		std::string protocol;
		
		//methods
		Connection(appWin* App, std::string protocol);
		void set_name(std::string name);
		std::string get_name();
		void set_url(std::string url);
		void start_download();
		static void* run_helper(void* arg);
		void* run();
		static size_t file_write(void *buffer, size_t size, size_t nmemb, void *stream);
		static int file_progress(Connection *con,  curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow);
		void set_progress(curl_off_t dlnow, curl_off_t dltotal);
};
