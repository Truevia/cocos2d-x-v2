#https://developer.android.com/ndk/guides/cpp-support
# gnustl_static / c++_static
APP_STL := c++_static

APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -fsigned-char #-std=c++11
APP_LDFLAGS := -latomic -landroid
#ndk-build 默认情况下仍将此决定留给 Clang，因此 ndk-build 用户应使用 APP_CPPFLAGS 来添加 -std=c++17 或任何所需内容。

#APP_ALLOW_MISSING_DEPS=true

#APP_ABI := armeabi-v7a
APP_SHORT_COMMANDS := true


ifeq ($(NDK_DEBUG),1)
  APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
