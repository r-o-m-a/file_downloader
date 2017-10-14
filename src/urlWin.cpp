#include "inc/urlWin.h"

urlWin::urlWin(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder> builder)
	:Gtk::Window(object)
{
	cancel = apply = nullptr;
	urlEntry = saveEntry = nullptr;
	directory = nullptr;
	builder->get_widget("cancel_button", cancel);
	builder->get_widget("apply_button", apply);
	builder->get_widget("url_entry", urlEntry);
	builder->get_widget("save_entry", saveEntry);
	builder->get_widget("directory", directory);
}

void urlWin::clean()
{
		urlEntry->set_text(Glib::ustring(""));
		saveEntry->set_text(Glib::ustring(""));
}

bool urlWin::check_strings()
{
		urlString = urlEntry->get_text();
		saveString = saveEntry->get_text();
		std::string url = urlString.raw();
		std::string save = saveString.raw();
		
		if((save.find("/") != std::string::npos) || save.empty())
			return false;
			
		if(directory->get_filename().empty())
			return false;
			
		struct dirent *entry;
		DIR *dir=opendir(directory->get_filename().c_str());
		
		while ( (entry = readdir(dir) ) != NULL)
		{
			if(!save.compare(std::string(entry->d_name)))
				return false;
		}		
		return true;
}
