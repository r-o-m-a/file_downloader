#pragma once
#include <gtkmm.h>

class attWin: public Gtk::Window
{
	private:
		Gtk::Button *okButton;
		Gtk::Label *attLabel; 
		Glib::ustring att_label;
		Glib::ustring limit_label;
		
	public:
		attWin(BaseObjectType* object,const  Glib::RefPtr<Gtk::Builder> builder);
		
		void ok_clicked();
		void set_att_label();
		void set_limit_label();
};

