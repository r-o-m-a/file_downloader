#pragma once
#include <gtkmm.h>

class Cols: public Gtk::TreeModel::ColumnRecord
{
	public:
		Cols();
		
		Gtk::TreeModelColumn<Glib::ustring> nameCol;
		Gtk::TreeModelColumn<gint> progressCol;
		Gtk::TreeModelColumn<Glib::ustring> statusCol;
		Gtk::TreeModelColumn<Glib::ustring> urlCol;
		Gtk::TreeModelColumn<Glib::ustring> destinationCol;
		Gtk::TreeModelColumn<Glib::ustring> protocolCol;
};
