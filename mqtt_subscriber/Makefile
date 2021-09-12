include $(TOPDIR)/rules.mk

PKG_NAME:=mqtt_subscriber
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/mqtt_subscriber
	CATEGORY:=Base system
	TITLE:=subscriber
	DEPENDS:=+libmosquitto +libuci +libsqlite3 +libjson-c +libcurl
endef

define Package/mqtt_subscriber/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mqtt $(1)/usr/bin
	$(INSTALL_BIN) ./files/mqtt_subs.init $(1)/etc/init.d/mqtt_subs
	$(INSTALL_CONF) ./files/mqtt_subs.config $(1)/etc/config/mqtt_subs
endef

$(eval $(call BuildPackage,mqtt_subscriber))