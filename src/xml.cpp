#include "inc/xml.h"

xml::xml()
{
	create_xml();
}

void xml::load_files(Glib::RefPtr<Gtk::ListStore> listStore, const Cols& columns)
{
	
	TiXmlHandle docHandle(&doc);
	
	TiXmlElement* child = docHandle.FirstChild("files").FirstChild("file").ToElement();
	
	for(child; child; child = child->NextSiblingElement() )
	{
		auto it = *(listStore->prepend());
		it[columns.nameCol] = child->Attribute("name");
		it[columns.urlCol] = child->Attribute("url");
		it[columns.destinationCol] = child->Attribute("path");
		it[columns.statusCol] = child->Attribute("status");
		it[columns.protocolCol] = child->Attribute("protocol");
		it[columns.progressCol] = atoi(child->Attribute("progress"));	
	}	
}

void xml::add_row_to_file(Gtk::TreeRow row, const Cols& columns)
{
	TiXmlHandle docHandle(&doc);
	TiXmlElement* files = docHandle.FirstChild("files").ToElement();
	TiXmlElement* new_element = new TiXmlElement("file");
	files->LinkEndChild(new_element);

	new_element->SetAttribute("name", Glib::ustring(row[columns.nameCol]));
	new_element->SetAttribute("url", Glib::ustring(row[columns.urlCol]));
	new_element->SetAttribute("path", Glib::ustring(row[columns.destinationCol]));
	new_element->SetAttribute("status", Glib::ustring(row[columns.statusCol]));
	new_element->SetAttribute("protocol", Glib::ustring(row[columns.protocolCol]));
	new_element->SetAttribute("progress", std::to_string(gint(row[columns.progressCol])));
	
	doc.SaveFile(dirPath.c_str());

}

void xml::delete_row_from_file(Glib::RefPtr<Gtk::ListStore> listStore, Gtk::TreeIter& it, const Cols& columns)
{
	auto child = listStore->children();
	int i = 0;
	for(auto iter = child.end(); iter != it; iter--)
	{
		i++;
	}
	i--;
	
	TiXmlHandle docHandle(&doc);
	TiXmlElement* files = docHandle.FirstChild("files").ToElement();
	TiXmlNode* child_to_remove = docHandle.FirstChild("files").Child("file", i).ToNode();
	files->RemoveChild(child_to_remove);
	doc.SaveFile(dirPath.c_str());
}

void xml::change_status(Glib::RefPtr<Gtk::ListStore> listStore, Gtk::TreeIter& it, const Cols& columns, std::string status_string)
{
	auto child = listStore->children();
	int i = 0;
	for(auto iter = child.end(); iter != it; iter--)
	{
		i++;
	}
	i--;
	
	TiXmlHandle docHandle(&doc);
	TiXmlElement* files = docHandle.FirstChild("files").ToElement();
	TiXmlElement* child_to_change = docHandle.FirstChild("files").Child("file", i).ToElement();
	child_to_change->SetAttribute("status", status_string);
	doc.SaveFile(dirPath.c_str());
}

void xml::save_progress(Glib::RefPtr<Gtk::ListStore> listStore, Gtk::TreeIter& it, const Cols& columns)
{
	auto child = listStore->children();
	int i = 0;
	for(auto iter = child.end(); iter != it; iter--)
	{
		i++;
	}
	i--;
	
	TiXmlHandle docHandle(&doc);
	TiXmlElement* files = docHandle.FirstChild("files").ToElement();
	TiXmlElement* child_to_change = docHandle.FirstChild("files").Child("file", i).ToElement();
	child_to_change->SetAttribute("progress", std::to_string(gint((*it)[columns.progressCol])));
	doc.SaveFile(dirPath.c_str());
}

void xml::create_xml()
{
	const char* homeDir;
	if((homeDir = getenv("HOME")) == NULL)
		 homeDir = getpwuid(getuid())->pw_dir;
		 
	dirPath = homeDir + std::string("/.file_manager");	 
		 
	if(!mkdir(dirPath.c_str(), 0755))
	{
		create_file();
		return;
	}
	
	DIR *dir = opendir(dirPath.c_str());
	if(dir)
	{
		struct dirent *ent;
		bool exist = false;
		while((ent = readdir(dir)) != NULL)
		{
			std::string fname = std::string(ent->d_name);
			if(!fname.compare("file_list.xml"))
				exist = true;
		}
		if (!exist)
			create_file();
		else
		{
			dirPath = dirPath + std::string("/file_list.xml");
			if(!doc.LoadFile(dirPath.c_str()))
				std::cout << "can't load file" << std::endl;
		}
	}
}

void xml::create_file()
{
	std::cout << "create_file()" << std::endl;
	dirPath = dirPath + std::string("/file_list.xml");
	std::ofstream ofs(dirPath.c_str());
	ofs.close();
	 
 	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc.LinkEndChild(decl); 
	
	TiXmlElement * root = new TiXmlElement( "files" );  
	doc.LinkEndChild(root);
	
	doc.SaveFile(dirPath.c_str()); 
}
