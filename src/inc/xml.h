#pragma once
#include <gtkmm.h>
#include <tinyxml.h>
#include <tinystr.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include "Cols.h"


class xml
{
	private:
		TiXmlDocument doc;
		std::string dirPath;
		
	public:
	xml();
	
	void create_xml();
	void create_file();
	void load_files(Glib::RefPtr<Gtk::ListStore> listStore, const Cols& columns);
	void add_row_to_file(Gtk::TreeRow row, const Cols& columns);
	void delete_row_from_file(Glib::RefPtr<Gtk::ListStore> listStore, Gtk::TreeIter& it, const Cols& columns);
	void change_status(Glib::RefPtr<Gtk::ListStore> listStore, Gtk::TreeIter& it, const Cols& columns, std::string status_string);
	void save_progress(Glib::RefPtr<Gtk::ListStore> listStore, Gtk::TreeIter& it, const Cols& columns);
};
