#include "inc/appWin.h"

#define DOWN_LIMIT 20
#define TIMER_FREQUENCY 700
#define TIME_TO_WAIT 100

appWin::appWin(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> builder)
	:Gtk::ApplicationWindow(cobject)
	
{
	//AppWin = nullptr;
	newFileButton = resumeButton = pauseButton = removeButton =  nullptr;
	
	//builder->get_widget("appWin", AppWin);
	builder->get_widget("new_file_button", newFileButton);
	builder->get_widget("resume_button", resumeButton);
	builder->get_widget("pause_button", pauseButton);
	builder->get_widget("remove_button", removeButton);
	
	treeView = nullptr;
	builder->get_widget("tree_view", treeView);
	treeSelection = treeView->get_selection();
	
	listStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(builder->get_object("list_store"));
	
	UrlWindow = nullptr;
	builder->get_widget_derived("UrlWindow", UrlWindow);

	AttWin = nullptr;
	builder->get_widget_derived("attentionWin", AttWin);
	
	connect_signals();
	
	queue = g_async_queue_new();
	//
	//fileList
	Xml.load_files(listStore, columns);

	curl_global_init(CURL_GLOBAL_ALL);
	
	for(auto it = listStore->children().begin(); it != listStore->children().end(); it++)
	{
		//check
		connectionVector.push_back(new Connection(this, (Glib::ustring)(*it)[columns.protocolCol]));
		connectionVector.back()->set_name(std::string((*it).get_value(columns.destinationCol) +  "/" + (*it).get_value(columns.nameCol)));
		connectionVector.back()->set_url(std::string((*it).get_value(columns.urlCol)));
		connectionVector.back()->dispatcher.connect(sigc::mem_fun(*this, &appWin::connection_end));
		if(!Glib::ustring((*it)[columns.statusCol]).compare("paused"))
			connectionVector.back()->progress = (*it)[columns.progressCol];
	}	
}

appWin::~appWin()
{
	for(auto it = connectionVector.begin(); it != connectionVector.end(); it++)
	{
		delete *it;
	}
}

void appWin::connect_signals()
{
	// newFileButton->signal_clicked().connect(sigc::bind<Gtk::Window*>(sigc::ptr_fun(&new_file_button_clicked), urlWindow));
	this->signal_delete_event().connect(sigc::mem_fun(*this, &appWin::delete_clicked));

	newFileButton->signal_clicked().connect(sigc::mem_fun(*this, &appWin::new_file_clicked));
	resumeButton->signal_clicked().connect(sigc::mem_fun(*this, &appWin::resume_clicked));
	pauseButton->signal_clicked().connect(sigc::mem_fun(*this, &appWin::pause_clicked));
	removeButton->signal_clicked().connect(sigc::mem_fun(*this, &appWin::remove_clicked));

	UrlWindow->cancel->signal_clicked().connect(sigc::mem_fun(*this, &appWin::cancel_clicked));
	UrlWindow->apply->signal_clicked().connect(sigc::mem_fun(*this, &appWin::apply_clicked));
	
	//slot for timer
	sigc::slot<bool> my_slot = sigc::mem_fun(*this, &appWin::timer);
	Glib::signal_timeout().connect(my_slot, TIMER_FREQUENCY, Glib::PRIORITY_DEFAULT);	
}

void appWin::new_file_clicked()
{
	newFileButton->set_sensitive(false);
	UrlWindow->clean();
	UrlWindow->show();
}

void  appWin::resume_clicked()
{
	if(is_empty())
		return;
	
	auto it = treeSelection->get_selected();
	
	int i = 0;
	if(!Glib::ustring((*it)[columns.statusCol]).compare("paused"))
	{
		auto iter = listStore->children().begin();
		for(; iter != it; iter++)
		{
			i++;
		}
		(*iter)[columns.statusCol] = "in progress";
		connectionVector.at(i)->start_download();
	}
}

void  appWin::pause_clicked()
{
	if(is_empty())
		return;
	
	auto it = treeSelection->get_selected();
	
	int i = 0;
	if(!Glib::ustring((*it)[columns.statusCol]).compare("in progress"))
	{
		auto iter = listStore->children().begin();
		for(; iter != it; iter++)
		{
			i++;
		}
		
		{
			std::lock_guard<std::mutex> lk(connectionVector.at(i)->c_mutex);
			connectionVector.at(i)->paused = true;
		}
		(*iter)[columns.statusCol] = "paused";
		Xml.change_status(listStore, iter, columns, "paused");
	}
}

void  appWin::remove_clicked()
{
	if(is_empty())
		return;
		
	auto it = treeSelection->get_selected();

	int i = 0;
	if(it)
	{
		Xml.delete_row_from_file(listStore, it, columns);
		
		for(auto iter = listStore->children().begin(); iter != it; iter++)
		{
			i++;
		}
		if(Glib::ustring((*it)[columns.statusCol]).compare("finished"))
		{
			if(!Glib::ustring((*it)[columns.statusCol]).compare("in progress"))
			{
				connectionVector.at(i)->paused = true;
				
				std::unique_lock<std::mutex> lk(connectionVector.at(i)->c_variable_mutex);
				connectionVector.at(i)->c_variable.wait_for(lk, std::chrono::duration<int, std::milli>(TIME_TO_WAIT));
			}
			
			std::remove(connectionVector.at(i)->get_name().c_str());		
		}
		delete connectionVector.at(i);
		connectionVector.erase(connectionVector.begin() + i);
		listStore->erase(it);
	}
}

void appWin::apply_clicked()
{
	Glib::ustring entryString;
	
	if(!UrlWindow->check_strings())
	{
		AttWin->set_att_label();
		AttWin->show();
		return;
	}
	else if(connectionVector.size() >= DOWN_LIMIT)
	{
		AttWin->set_limit_label();
		AttWin->show();
		return;
	}
		
	auto it = *(listStore->prepend());
	fulfil_fields(it);
	Xml.add_row_to_file(it, columns);
	
	//setting connection
	connectionVector.insert(connectionVector.begin(), new Connection(this, (Glib::ustring)(*it)[columns.protocolCol]));
	connectionVector.at(0)->set_name(std::string(it.get_value(columns.destinationCol) +  "/" + it.get_value(columns.nameCol)));
	connectionVector.at(0)->set_url(std::string(it.get_value(columns.urlCol)));
	connectionVector.at(0)->dispatcher.connect(sigc::mem_fun(*this, &appWin::connection_end));
	connectionVector.at(0)->start_download();
	
	//hide url window
	UrlWindow->hide();
	newFileButton->set_sensitive(true);	
}

void appWin::cancel_clicked()
{
	UrlWindow->hide();
	newFileButton->set_sensitive(true);
}

bool appWin::delete_clicked(GdkEventAny* any_event)
{
	int i = 0;
	for(auto it = listStore->children().begin(); it != listStore->children().end(); it++, i++)
	{
		if(!Glib::ustring((*it)[columns.statusCol]).compare("in progress"))
		{
			Xml.change_status(listStore, it, columns, "paused");
		}
			Xml.save_progress(listStore, it, columns);
	}
	return false;
}

bool appWin::is_empty()
{
	if(listStore->children().begin())
		return false;
		
	else
		return true;
}

void appWin::fulfil_fields(Gtk::TreeRow row)
{
	row[columns.nameCol] = UrlWindow->saveString;
	row[columns.urlCol] = UrlWindow->urlString;
	row[columns.progressCol] = 0;
	
	if(is_http(UrlWindow->urlString))
		row[columns.protocolCol] = "http";
	else
		row[columns.protocolCol] = "ftp";
		
	row[columns.destinationCol] = UrlWindow->directory->get_filename();
	row[columns.statusCol] = "in progress";
}

bool appWin::timer()
{
	int i = 0;
	for(auto it = listStore->children().begin(); it != listStore->children().end(); it++, i++)
	{
		if(!Glib::ustring((*it)[columns.statusCol]).compare("in progress"))
		{
			std::lock_guard<std::mutex> lk( connectionVector.at(i)->c_mutex);
			(*it)[columns.progressCol] = (int)connectionVector.at(i)->progress;
		}
	}
	return true;
}

void appWin::connection_end()
{
	int i = 0;
	Connection* con = (Connection*)g_async_queue_pop(queue);

	auto iter = listStore->children().begin();
	for(auto it = connectionVector.begin(); it != connectionVector.end(); it++, iter++, i++)
	{
		if( (*it) == con)
			break;
	}

	(*iter)[columns.statusCol] = con->status;
	Xml.change_status(listStore, iter, columns, con->status);
	
	if(!con->status.compare("finished"))
		(*iter)[columns.progressCol] = 100;
	else if(!con->status.compare("failed"))
		(*iter)[columns.progressCol] = 0;
}

bool appWin::is_http(std::string to_check)
{
	if(to_check.size() > 2)
	{
		if(to_check[0] == 'f' &&
		   to_check[1] == 't' &&
		   to_check[2] == 'p')
		   return false;
		else
			return true;
	}
	else
		return true;
}
