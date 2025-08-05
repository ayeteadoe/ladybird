#include <AK/StringView.h>
#include <AK/Vector.h>
#include <LibCore/System.h>
#include <LibCore/TimeZoneWatcher.h>
#include <LibTest/TestCase.h>

#include <AK/Windows.h>

using namespace Core;

TEST_CASE(timezone_watcher)
{
    auto time_zone_watcher = TRY_OR_FAIL(TimeZoneWatcher::create());
    time_zone_watcher->on_time_zone_changed = []() {
        outln("Timezone changed!");
    };

    outln("Listening for system time zone changes...");
    MSG msg;
    BOOL bRet;

    // TODO: Not receiving any message when we change the timezone via powershell, see https://devblogs.microsoft.com/oldnewthing/20160101-00/?p=92761 may need an actual top level window or something? Not sure

    while ((bRet = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            outln("GetMessageW failed with error: {}", GetLastError());
            break;
        } else {

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}
