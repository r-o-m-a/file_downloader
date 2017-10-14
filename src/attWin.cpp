#include "inc/attWin.h"

attWin::attWin(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder> builder)
	:Gtk::Window(obj)
{
	att_label = "There' s something wrong with an input information!\n Please, check it again!";
	limit_label = "There' are too much downloads! \nPlease, delete some of them at first!";
	
	okButton = nullptr;
	attLabel = nullptr;
	
	builder->get_widget("ok_button", okButton);
	builder->get_widget("att_label", attLabel);
	
	okButton->signal_clicked().connect(sigc::mem_fun(*this, &attWin::ok_clicked));
}

void attWin::ok_clicked()
{
	this->hide();
}

void attWin::set_att_label()
{
	attLabel->set_text(att_label);
}

void attWin::set_limit_label()
{
	attLabel->set_text(limit_label);
}
