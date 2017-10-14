#pragma once
#include <gtkmm.h>
#include <cstdio>
#include <iostream>
#include <tinyxml.h>
#include <curl/curl.h>
#include <glib.h>
#include "urlWin.h"
#include "Cols.h"
#include "attWin.h"
#include "xml.h"
#include "connection.h"

class Connection;

class appWin: public Gtk::ApplicationWindow
{
	private:
		Gtk::ToolButton *newFileButton, *resumeButton, *pauseButton,  *removeButton;
		
		Glib::RefPtr<Gtk::ListStore> listStore;
		
		
		Gtk::TreeView *treeView;
		Gtk::TreeModel::Path activeRow;
		
		Glib::RefPtr<Gtk::TreeSelection> treeSelection;
		
		Cols columns;
		std::vector<Connection* > connectionVector;
		
		//xml
		xml Xml;
				
	public:
		GAsyncQueue* queue;
		
		appWin(BaseObjectType* cobject,const Glib::RefPtr<Gtk::Builder> builder);
		~appWin();
		void connect_signals();
		
		urlWin* UrlWindow;
		attWin* AttWin;
		
		//signal handlers
		void new_file_clicked();
		void resume_clicked();
		void pause_clicked();
		void reload_clicked();
		void remove_clicked();
		void remove_all_clicked();
		
		//other windows' handlers
		
		void apply_clicked();
		void cancel_clicked();
		void http_clicked();
		void ftp_clicked();
		bool delete_clicked(GdkEventAny* any_event);
		
		//additional funcs
		bool is_empty();
		void fulfil_fields(Gtk::TreeRow row);
		
		//timer
		bool timer();
		
		void connection_end();
		
		bool is_http(std::string to_check);
};

