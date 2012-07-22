// Standard headers
#include <cassert>
#include <iostream>

// Spitfire headers
#include <spitfire/spitfire.h>

// Medusa headers
#include "gtkmmicontheme.h"

namespace medusa
{
  // ** cGtkmmIconTheme

  cGtkmmIconTheme::cGtkmmIconTheme()
  {
    icon_theme = Gtk::IconTheme::get_default();
  }

  void cGtkmmIconTheme::LoadStockIconWithSizePixels(const std::string& sStockIconName, size_t sizePixels, Gtk::Image& image)
  {
    std::cout<<"cGtkmmIconTheme::LoadStockIcon"<<std::endl;
    image.set(icon_theme->load_icon(sStockIconName.c_str(), sizePixels, Gtk::ICON_LOOKUP_USE_BUILTIN));
  }

  void cGtkmmIconTheme::LoadStockIcon(const std::string& sStockIconName, Gtk::Image& image)
  {
    std::cout<<"cGtkmmIconTheme::LoadStockIcon"<<std::endl;
    image.set(icon_theme->load_icon(sStockIconName.c_str(), 32, Gtk::ICON_LOOKUP_USE_BUILTIN));
  }

  void cGtkmmIconTheme::LoadStockIconRotatedClockwise(const std::string& sStockIconName, Gtk::Image& image)
  {
    std::cout<<"cGtkmmIconTheme::LoadStockIconRotatedClockwise"<<std::endl;

    // This doesn't seem to work
    //Gtk::StockItem item;
    //Gtk::Stock::lookup(idStockIcon, item);

    Glib::RefPtr<Gdk::Pixbuf> pixbuf = icon_theme->load_icon(sStockIconName.c_str(), 32, Gtk::ICON_LOOKUP_USE_BUILTIN);

    ASSERT(pixbuf);

    // Rotate the pixbuf and assign it to our image
    image.set(pixbuf->rotate_simple(Gdk::PixbufRotation::PIXBUF_ROTATE_CLOCKWISE));
  }
}
