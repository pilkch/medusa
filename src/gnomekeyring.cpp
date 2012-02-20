// Gnome headers
#include <gnome-keyring.h>

// Medusa headers
#include "gnomekeyring.h"
#include "util.h"

namespace medusa
{
  class cGnomeKeyRingFindResult
  {
  public:
    cGnomeKeyRingFindResult();
    ~cGnomeKeyRingFindResult();

    spitfire::util::cSignalObject soDone;
    spitfire::string_t sPassword;
  };

  class cGnomeKeyRingSaveResult
  {
  public:
    cGnomeKeyRingSaveResult();

    spitfire::util::cSignalObject soDone;
  };


  // ** cGnomeKeyRingFindResult

  cGnomeKeyRingFindResult::cGnomeKeyRingFindResult() :
    soDone("cGnomeKeyRingFindResult::soDone")
  {
  }

  cGnomeKeyRingFindResult::~cGnomeKeyRingFindResult()
  {
    medusa::util::ClearPassword(sPassword);
  }

  // ** cGnomeKeyRingSaveResult

  cGnomeKeyRingSaveResult::cGnomeKeyRingSaveResult() :
    soDone("cGnomeKeyRingSaveResult::soDone")
  {
  }


  // ** cGnomeKeyRing

  void FoundPassword(GnomeKeyringResult result, const gchar* szPasswordUTF8, gpointer pUserData)
  {
    cGnomeKeyRingFindResult* pThis = static_cast<cGnomeKeyRingFindResult*>(pUserData);
    ASSERT(pThis != nullptr);

    if (result != GNOME_KEYRING_RESULT_OK) std::cout<<"cGnomeKeyRing::FoundPassword Couldn't find password, error=\""<<gnome_keyring_result_to_message(result)<<"\""<<std::endl;

    pThis->sPassword = spitfire::string::ToString_t(szPasswordUTF8);

    pThis->soDone.Signal();
  }

  void cGnomeKeyRing::LoadPassword(spitfire::string_t& sPassword)
  {
    cGnomeKeyRingFindResult result;
    void* pThis = &result;
    gnome_keyring_find_password(
      GNOME_KEYRING_NETWORK_PASSWORD,  // The password type
      FoundPassword,                   // A function called when complete
      pThis, NULL,                     // User data for callback, and destroy notify

      // These are the attributes
      //"user", "me",
      //"server", "gnome.org",

      NULL // Always end with NULL
    );

    result.soDone.WaitForever();
    sPassword = result.sPassword;
  }

  void StoredPassword(GnomeKeyringResult result, gpointer pUserData)
  {
    cGnomeKeyRingSaveResult* pThis = static_cast<cGnomeKeyRingSaveResult*>(pUserData);
    ASSERT(pThis != nullptr);

    if (result != GNOME_KEYRING_RESULT_OK) std::cout<<"cGnomeKeyRing::StoredPassword Couldn't save password, error=\""<<gnome_keyring_result_to_message(result)<<"\""<<std::endl;

    pThis->soDone.Signal();
  }

  void cGnomeKeyRing::SavePassword(const spitfire::string_t& sCaption, const spitfire::string_t& sPassword)
  {
    std::string sPasswordUTF8(spitfire::string::ToUTF8(sPassword));
    std::string sCaptionUTF8(spitfire::string::ToUTF8(sCaption));
    cGnomeKeyRingSaveResult result;
    void* pThis = &result;
    gnome_keyring_store_password(
      GNOME_KEYRING_NETWORK_PASSWORD, // The password type
      GNOME_KEYRING_DEFAULT,          // Where to save it
      sCaptionUTF8.c_str(),           // Password description, displayed to user
      sPasswordUTF8.c_str(),          // The password itself
      StoredPassword,                 // A function called when complete
      pThis, NULL,                    // User data for callback, and destroy notify

      // These are the attributes
      //"user", "me",
      //"server", "gnome.org",

      NULL // Always end with NULL
    );

    result.soDone.WaitForever();

    // Clear our temporary copy of the password
    medusa::util::ClearPassword(sPasswordUTF8);
  }
}
