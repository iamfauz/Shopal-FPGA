-- Data below used for making wireless connection to router for WIFI
-- SSID = "SM-G960W1733"
-- SSID_PASSWORD = "qrvg9339"

station_cfg={}
station_cfg.ssid="SM-G960W1733"
station_cfg.pwd="qrvg9339"
station_cfg.save=true

-- configure ESP as a station
wifi.setmode(wifi.STATION)
wifi.sta.config(station_cfg)	-- Configure the wireless network, wifi
wifi.sta.autoconnect(1)

-- pause for connection to take place - adjust time delay if necessary or repeat until connection made
tmr.delay(1000000) -- wait 1,000,000 us = 1 second





-- SSID = "SM-G960W1733"
-- SSID_PASSWORD = "qrvg9339"

-- -- I understand from students who have tried, that this also works, using UBC Visitor wireless connection
-- -- although I haven't tried it

-- -- SSID= "ubcvisitor"
-- -- SSID_PASSWORD = ""

-- -- configure ESP as a station
-- wifi.setmode(wifi.STATION)
-- wifi.sta.config(SSID,SSID_PASSWORD)
-- wifi.sta.autoconnect(1)