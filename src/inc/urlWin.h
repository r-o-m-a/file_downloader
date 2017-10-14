#pragma once
#include <gtkmm.h>
#include <iostream>
#include <dirent.h>

class urlWin: public Gtk::Window
{
	
	public:
		urlWin(BaseObjectType* object,const  Glib::RefPtr<Gtk::Builder> builder);
		
		Gtk::Button *cancel, *apply;
		Gtk::Entry *urlEntry, *saveEntry;
		Gtk::FileChooserButton *directory;
		
		Glib::ustring urlString, saveString;
		
		bool check_strings();
		
		void clean();
};
