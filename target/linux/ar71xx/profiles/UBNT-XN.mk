define Profile/UBNTXN
  NAME:=Ubiquiti XN products
  PACKAGES:=kmod-ebtables kmod-ipt-conntrack kmod-ipt-conntrack-extra kmod-ipt-nat kmod-ag7240 kmod-ag7240-s26phy kmod-ppp kmod-mppe kmod-pppoe kmod-ar7240-gpio kmod-rssi-leds kmod-ath-11n
endef

define Profile/UBNTXN/Description
	Package set compatible with the Ubiquiti XN (Python+Merlin based products).
endef
$(eval $(call Profile,UBNTXN))

