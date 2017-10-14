#include "inc/connection.h"

Connection::Connection(appWin* App, std::string protocol):
	dispatcher()
{
	progress = 0;
	paused = false;
	downloaded = false;
	status = "in progress";
	iCountBadPerform = 3;
	start_from = 0;
	this->App = App;
	this->protocol = protocol;
}

void Connection::start_download()
{
	 int error;
     error = pthread_create(&id, NULL, &Connection::run_helper, this);

    if(0 != error)
      std::cout << "couldn' t run thread" << std::endl;
}


void* Connection::run_helper(void* arg)
{
	((Connection*)arg)->run();
	return NULL;
}

void* Connection::run()
{
	CURL *curl;
	CURLcode res; 

    fp = fopen(name.c_str(), "ab");
	
	curl = curl_easy_init();
	if(curl)
	{
		do
		{
			
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Connection::file_write);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS,  0);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); 
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10 );
			curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1000); //bytes/sec
			curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 10);
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, &Connection::file_progress);
			curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
			stat(name.c_str(), &statbuf);
			curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, start_from = statbuf.st_size);
			res = curl_easy_perform(curl);
		}
		while((CURLE_OK != res) && iCountBadPerform--);
		
		iCountBadPerform = 3;
		long response;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response);
		std::cout << name << "response == " << response << std::endl;

		if(!protocol.compare("http"))
		{
			switch(response)
			{
				case 200:
				case 206:
					status = "finished";
					break;
				case 404:
				case 301:
				case 300:
					status = "failed";
					break;
				default:
					if(start_from > 0 || downloaded)
					{
						downloaded = false;
						status = "paused";
					}
					else
						status = "failed";
					break;
			}
		}
		
		else
		{
			switch(response)
			{
				case 226:
					status = "finished";
					break;
				default:
					if(start_from > 0 || downloaded)
					{
						downloaded = false;
						status = "paused";
					}
					else
						status = "failed";
					break;
			}
		}
				
		g_async_queue_push(App->queue, this);
		dispatcher.emit();
		curl_easy_cleanup(curl);
	}
	fclose(fp);
	pthread_exit(NULL);
}

int Connection::file_progress(Connection *con,  curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow)
{
    if (dlnow && dltotal)
    {
		con->set_progress(dlnow, dltotal);
    }   
    return 0;
}

size_t Connection::file_write(void *buffer, size_t size, size_t nmemb, void *stream)
{    
    return fwrite(buffer, size, nmemb, (FILE*)stream); 
}

void Connection::set_progress(curl_off_t dlnow, curl_off_t dltotal)
{
	downloaded = true;
	std::lock_guard<std::mutex> lk(c_mutex);
	progress = (double)(100*(start_from + dlnow)/(start_from + dltotal));
	if(paused)
	{
		paused = false;
		fclose(fp);
		c_variable.notify_all();
		pthread_exit(NULL);
	}
}

void Connection::set_name(std::string name)
{
	this->name = name;
}

std::string Connection::get_name()
{
	return name;
}

void Connection::set_url(std::string url)
{
	this->url = url;
}
