// Spitfire headers
#include <spitfire/communication/network.h>

// Medusa headers
#include "gtkmmpreferencesdialog.h"

namespace medusa
{
cGtkmmPreferencesDialog::cGtkmmPreferencesDialog(cSettings& _settings, Gtk::Window& parent) :
  Gtk::Dialog("Preferences", parent, true),
  settings(_settings),
  groupPlayback("Playback"),
  playbackRepeat("Repeat"),
  playbackNotifyOnSongChange("Notify on song change"),
  playbackNextSongOnMoveToTrash("Skip to next song when the current song is moved to the trash"),
  groupLastfm("Last.fm"),
  lastfmEnabled("Use Last.fm"),
  lastfmTable(2, 2),
  lastfmUserNameDescription("User:"),
  lastfmPasswordDescription("Password:"),
  lastfmNewToLastfmDescription("New to Last.fm?"),
  lastfmSignUpForAnAccount("http://www.last.fm/join", "Sign up for an account"),
  groupWebServer("Playback"),
  webServerEnabled("Enable web server"),
  pOkButton(nullptr)
{
  set_border_width(5);

  set_resizable();

  signal_response().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnResponse));

  Gtk::Box* pBox = get_vbox();

  pBox->set_border_width(10);

  // Playback
  playbackRepeat.set_active(settings.IsRepeat());
  playbackNotifyOnSongChange.set_active(settings.IsNotifyOnSongChange());
  playbackNextSongOnMoveToTrash.set_active(settings.IsNextSongOnMoveToTrash());

  pBox->pack_start(groupPlayback, Gtk::PACK_SHRINK);
  groupPlayback.add(boxPlayback);
  boxPlayback.pack_start(playbackRepeat, Gtk::PACK_SHRINK);
  boxPlayback.pack_start(playbackNotifyOnSongChange, Gtk::PACK_SHRINK);
  boxPlayback.pack_start(playbackNextSongOnMoveToTrash, Gtk::PACK_SHRINK);

  // Last.fm
  lastfmEnabled.signal_toggled().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnEnableControls));

  lastfmEnabled.set_active(settings.IsLastFMEnabled());
  lastfmUserName.set_text(spitfire::string::ToUTF8(settings.GetLastFMUserName()).c_str());
  lastfmPassword.set_text(spitfire::string::ToUTF8(settings.GetLastFMPassword()).c_str());

  const bool bEnabled = false;
  lastfmUserNameDescription.set_sensitive(bEnabled);
  lastfmUserName.set_sensitive(bEnabled);
  lastfmUserName.signal_changed().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnEnableControls));
  lastfmUserName.set_activates_default(true);
  lastfmPasswordDescription.set_sensitive(bEnabled);
  lastfmPassword.set_sensitive(bEnabled);
  lastfmPassword.signal_changed().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnEnableControls));
  lastfmPassword.set_activates_default(true);

  pBox->pack_start(groupLastfm, Gtk::PACK_SHRINK);
  groupLastfm.add(boxLastfm);
  boxLastfm.pack_start(lastfmEnabled, Gtk::PACK_SHRINK);
  boxLastfm.pack_start(lastfmTable);
  lastfmTable.attach(lastfmUserNameDescription, 0, 1, 0, 1, Gtk::SHRINK);
  lastfmTable.attach(lastfmUserName, 1, 2, 0, 1);
  lastfmTable.attach(lastfmPasswordDescription, 0, 1, 1, 2, Gtk::SHRINK);
  lastfmTable.attach(lastfmPassword, 1, 2, 1, 2);
  lastfmPassword.set_visibility(false);
  boxLastfm.pack_start(boxLastfmSignup, Gtk::PACK_SHRINK);
  boxLastfmSignup.pack_start(lastfmNewToLastfmDescription, Gtk::PACK_SHRINK);
  boxLastfmSignup.pack_start(lastfmSignUpForAnAccount, Gtk::PACK_SHRINK);

  // Web server
  webServerEnabled.set_active(settings.IsWebServerEnabled());

  pBox->pack_start(groupWebServer, Gtk::PACK_SHRINK);
  groupWebServer.add(boxWebServer);
  boxWebServer.pack_start(webServerEnabled, Gtk::PACK_SHRINK);

  std::list<spitfire::network::cIPAddress> addresses;
  if (spitfire::network::GetIPAddressesOfNetworkInterfaces(addresses)) {
    std::list<spitfire::network::cIPAddress>::const_iterator iter(addresses.begin());
    const std::list<spitfire::network::cIPAddress>::const_iterator iterEnd(addresses.end());
    while (iter != iterEnd) {
      const string_t sAddress = iter->ToString();
      LOG<<"Interface "<<sAddress<<std::endl;

      // Create a new link and add it to our dialog
      Gtk::LinkButton* pLinkButton = new Gtk::LinkButton("http://" + sAddress + ":" + spitfire::string::ToString(MEDUSA_WEB_SERVER_PORT), "Test the web server");
      boxWebServer.pack_start(*pLinkButton, Gtk::PACK_SHRINK);

      // Add the link to our list
      webServerLinks.push_back(pLinkButton);

      iter++;
    }
  }


  // Add separator
  pBox->pack_start(separator, Gtk::PACK_SHRINK);

  // Add standard buttons
  add_button(Gtk::Stock::CANCEL, Gtk::ResponseType::RESPONSE_CANCEL);
  pOkButton = add_button(Gtk::Stock::OK, Gtk::ResponseType::RESPONSE_OK);
  set_default_response(Gtk::ResponseType::RESPONSE_OK);

  // Make sure that our controls are in the correct state
  OnEnableControls();

  show_all_children();
}

  cGtkmmPreferencesDialog::~cGtkmmPreferencesDialog()
  {
    const size_t n = webServerLinks.size();
    for (size_t i = 0; i < n; i++) delete webServerLinks[i];
  }

void cGtkmmPreferencesDialog::OnResponse(int response_id)
{
  if (response_id == Gtk::ResponseType::RESPONSE_OK) {
    settings.SetRepeat(playbackRepeat.get_active());
    settings.SetNotifyOnSongChange(playbackNotifyOnSongChange.get_active());
    settings.SetNextSongOnMoveToTrash(playbackNextSongOnMoveToTrash.get_active());

    settings.SetLastFMEnabled(lastfmEnabled.get_active());
    settings.SetLastFMUserName(spitfire::string::ToString_t(lastfmUserName.get_text()));
    settings.SetLastFMPassword(spitfire::string::ToString_t(lastfmPassword.get_text()));

    settings.SetWebServerEnabled(webServerEnabled.get_active());

    settings.Save();
  }
}

void cGtkmmPreferencesDialog::OnEnableControls()
{
  bool bEnabled = lastfmEnabled.get_active();
  lastfmUserNameDescription.set_sensitive(bEnabled);
  lastfmUserName.set_sensitive(bEnabled);
  lastfmPasswordDescription.set_sensitive(bEnabled);
  lastfmPassword.set_sensitive(bEnabled);

  if (pOkButton != nullptr) pOkButton->set_sensitive(!bEnabled || (bEnabled && !lastfmUserName.get_text().empty() && !lastfmPassword.get_text().empty()));

  bEnabled = webServerEnabled.get_active();
  const size_t n = webServerLinks.size();
  for (size_t i = 0; i < n; i++) webServerLinks[i]->set_sensitive(bEnabled);
}

bool cGtkmmPreferencesDialog::Run()
{
  return (run() == Gtk::ResponseType::RESPONSE_OK);
}
}
