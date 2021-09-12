m = Map("mqtt_subs")
local certs = require "luci.model.certificate"
st = m:section(NamedSection, "mqtt_settings", "mqtt", translate("Settings"), translate("Settings for subscribtion service"))

flag = st:option(Flag, "enable", "Enable", translate("Enable subscription service"))

remote_address = st:option(Value, "remote_address", translate("Remote address"), translate("Select remote bridge address"))
remote_address.datatype = "host"
remote_address.placeholder = "0.0.0.0"
remote_address:depends("enable", "1")
remote_address.parse = function(self, section, novld, ...)
        local enable = luci.http.formvalue("cbid.mosquitto_sub.mosquitto_sub.enable")
        local value = self:formvalue(section)
        if enable and (value == nil or value == "") then
                self:add_error(section, "invalid", translate("Error: remote address is empty"))
        end
        Value.parse(self, section, novld, ...)
end

remote_port = st:option(Value, "remote_port", translate("Remote port"), translate("Select remote bridge port"))
remote_port.datatype = "port"
remote_port.placeholder = "1883"
remote_port:depends("enable", "1")
remote_port.parse = function(self, section, novld, ...)
        local enable = luci.http.formvalue("cbid.mosquitto_sub.mosquitto_sub.enable")
        local value = self:formvalue(section)
        if enable and (value == nil or value == "") then
                self:add_error(section, "invalid", translate("Error: remote port is empty"))
        end
        Value.parse(self, section, novld, ...)
end

remote_username = st:option(Value, "remote_username", translate("Remote username"), translate("Enter username of remote host"))
remote_username.datatype = "credentials_validate"
remote_username.validator_hint = translate("All characters are allowed except ` and space. Length must be between 1 and 256 characters")
remote_username.placeholder = "username"
remote_username:depends("enable", "1")
remote_username.parse = function(self, section, novld, ...)
        local enable = luci.http.formvalue("cbid.mosquitto_sub.mosquitto_sub.enable")
        local value = self:formvalue(section)
        if enable and (value == nil or value == "") then
                self:add_error(section, "invalid", translate("Error: username is not specified"))
        end
        Value.parse(self, section, novld, ...)
end

remote_password = st:option(Value, "remote_password", translate("Remote password"), translate("Enter password of remote host"))
remote_password.datatype = "credentials_validate"
remote_password.validator_hint = translate("All characters are allowed except ` and space. Length must be between 1 and 256 characters")
remote_password.placeholder = "password"
remote_password:depends("enable", "1")
remote_password.password = true
remote_password.parse = function(self, section, novld, ...)
        local enable = 
        luci.http.formvalue("cbid.mosquitto_sub.mosquitto_sub.enable")
        local value = self:formvalue(section)
        if enable and (value == nil or value == "") then
                self:add_error(section, "invalid", translate("Error: password is specified"))
        end
        Value.parse(self, section, novld, ...)
end

FileUpload.size = "262144"
FileUpload.sizetext = translate("Selected file is too large, max 256 KiB")
FileUpload.sizetextempty = translate("Selected file is empty")
FileUpload.unsafeupload = true

tls_enabled = st:option(Flag, "tls", translate("TLS"), translate("Select to enable TLS encryption"))
tls_enabled:depends("enable", "1")

tls_type = st:option(ListValue, "tls_type", translate("TLS Type"), translate("Select the type of TLS encryption"))
tls_type:depends({enable = "1", tls = "1"})
tls_type:value("cert", translate("Certificate based"))
tls_type:value("psk", translate("Pre-Shared-Key based"))
--------------------
local certificates_link = luci.dispatcher.build_url("admin", "system", "admin", "certificates")
o = st:option(Flag, "_device_files", translate("Certificate files from device"), translatef("Choose this option if you want to select certificate files from device.\
																					Certificate files can be generated <a class=link href=%s>%s</a>", certificates_link, translate("here")))
o:depends({tls = "1", tls_type = "cert"})
local cas = certs.get_ca_files().certs
local certificates = certs.get_certificates()
local keys = certs.get_keys()

tls_cafile = st:option(FileUpload, "cafile", translate("CA file"), "")
tls_cafile:depends({tls = "1", _device_files="", tls_type = "cert"})

tls_certfile = st:option(FileUpload, "certfile", translate("Certificate file"), "")
tls_certfile:depends({tls = "1", _device_files="", tls_type = "cert"})

tls_keyfile = st:option(FileUpload, "keyfile", translate("Key file"), "")
tls_keyfile:depends({tls = "1", _device_files="", tls_type = "cert"})

tls_cafile = st:option(ListValue, "_device_cafile", translate("CA file"), "")
tls_cafile:depends({tls = "1", _device_files="1", tls_type = "cert"})

if #cas > 0 then
	for _,ca in pairs(cas) do
		tls_cafile:value("/etc/certificates/" .. ca.name, ca.name)
	end
else 
	tls_cafile:value("", translate("-- No files available --"))
end

function tls_cafile.write(self, section, value)
	m.uci:set(self.config, section, "cafile", value)
end

tls_cafile.cfgvalue = function(self, section)
	return m.uci:get(m.config, section, "cafile") or ""
end

tls_certfile = st:option(ListValue, "_device_certfile", translate("Certificate file"), "")
tls_certfile:depends({tls = "1", _device_files="1", tls_type = "cert"})

if #certificates > 0 then
	for _,certificate in pairs(certificates) do
		tls_certfile:value("/etc/certificates/" .. certificate.name, certificate.name)
	end
else 
	tls_certfile:value("", translate("-- No files available --"))
end

function tls_cafile.write(self, section, value)
	m.uci:set(self.config, section, "certfile", value)
end

tls_cafile.cfgvalue = function(self, section)
	return m.uci:get(m.config, section, "certfile") or ""
end

tls_keyfile = st:option(ListValue, "_device_keyfile", translate("Key file"), "")
tls_keyfile:depends({tls = "1", _device_files="1", tls_type = "cert"})

if #keys > 0 then
	for _,key in pairs(keys) do
		tls_keyfile:value("/etc/certificates/" .. key.name, key.name)
	end
else 
	tls_keyfile:value("", translate("-- No files available --"))
end

function tls_keyfile.write(self, section, value)
	m.uci:set(self.config, section, "keyfile", value)
end

tls_keyfile.cfgvalue = function(self, section)
	return m.uci:get(m.config, section, "keyfile") or ""
end


o = st:option(Value, "psk", translate("Pre-Shared-Key"), translate("The pre-shared-key in hex format with no leading “0x”"))
o.datatype = "lengthvalidation(0, 128)"
o.placeholder = "Key"
o:depends({tls = "1", tls_type = "psk"})

o = st:option(Value, "identity", translate("Identity"), translate("Specify the Identity"))
o.datatype = "uciname"
o.placeholder = "Identity"
o:depends({tls = "1", tls_type = "psk"})

return m