#pragma once
namespace msw::tray {
class Tray;

class Listener {

	Tray* tray_;
public:
  Listener(Tray* tray);
  
};
}
