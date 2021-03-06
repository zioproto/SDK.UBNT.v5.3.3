#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 15383 2009-04-24 13:09:33Z nico $

include $(TOPDIR)/rules.mk

PKG_NAME:=peerguardian
PKG_VERSION:=1.5beta
PKG_RELEASE:=1.1

PKG_SOURCE:=pglinux-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/peerguardian
PKG_MD5SUM:=0fb2bc5501b031604fc56eec3bd35fa4

PKG_BUILD_DIR:=$(BUILD_DIR)/pglinux-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=iptables

include $(INCLUDE_DIR)/package.mk

define Package/peerguardian
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:= +libncurses +libpthread +uclibcxx +libnetfilter-queue
  TITLE:=PeerGuardian for Linux
  URL:=http://phoenixlabs.org/
endef

define Package/peerguardian/description
 PeerGuardian helps protect your privacy by blocking many ranges  of aggressive 
 IPs while you use P2P.
endef

define Package/peerguardian/conffiles
/etc/PG.conf
/etc/p2p.p2b.p2p
endef

CONFIGURE_VARS+= \
	CXXFLAGS="$$$$CXXFLAGS $$$$CPPFLAGS -fno-builtin -fno-rtti -I$(STAGING_DIR)/usr/include/libnetfilter_queue/" \
	LIBS="-nodefaultlibs -luClibc++ -Wl,-Bstatic,-lstdc++,-Bdynamic -lm -lgcc" \

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CXXLD="$(TARGET_CC)"
endef

define Package/peerguardian/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/PG.conf $(1)/etc/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/p2p.p2b.p2p $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/peerguardnf $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pgtext $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/peerguardian.init $(1)/etc/init.d/peerguardian
endef

$(eval $(call BuildPackage,peerguardian))
