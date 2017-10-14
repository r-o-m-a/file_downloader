#include "inc/appWin.h"

int main(int argc, char *argv[])
{
  //main application
  Glib::RefPtr<Gtk::Application> app =
    Gtk::Application::create(argc, argv,
      "org.gtkmm.examples.base");
      
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("/usr/share/fd.glade");
  
  //main window class
  appWin* AppWin = nullptr;
  
  builder->get_widget_derived("appWin", AppWin);
  
  return app->run(*AppWin);
}
