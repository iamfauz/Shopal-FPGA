-- Shopal HTTPS requests

TWILIO_ACCOUNT_SID = "AC16705da024de2ef2dc7630f405557d89"
TWILIO_TOKEN =       "b164d3bb00c41eda2ce6a5a2264629b0"

-- Unfortunately, the Wi-FI dongle can only make unsecured HTTP requests, but Twilio requires 
-- secured HTTPS requests, so we will use a relay website to convert HTTP requests into HTTPS requests
-- visit http://iot-https-relay.appspot.com/ to learn more about this service
-- Please be sure to understand the security issues of using this relay app and use at your own risk.

-- this is the web address of the relay web site that our dongle sends the initial HTTP request to
HOST = "iot-https-relay.appspot.com" 

-- The following variable defines the TWILIO web site that we will connect to
-- use the first one if you want to send a text to a cell phone
-- use the second (commented out) one if you want to make a call to a cell phone - that's the only change
URI = "/twilio/Messages.json"
--URI = "/twilio/Calls.json"

twilio = ""

-- get database
function get_db()
	twilio = " - \nHey you're near your Grocery Store:"
	http.get('https://shopal391.firebaseio.com/shopal_list.json', nil, function(code, data)
		if (code < 0) then
			--print("HTTP request failed")
		else
			for w in string.gmatch (data, "\"name\":\"([%w%s]+)\"" ) do	
				twilio = twilio .. "\n  - " .. w
			end
			twilio = twilio .. "\n Have fun shopping!"
		end
	end)
end

function build_post_request(host, uri, data_table)

     data = ""

     for param,value in pairs(data_table) do
          data = data .. param.."="..value.."&"
     end

     request = "POST "..uri.." HTTP/1.1\r\n"..
     "Host: "..host.."\r\n"..
     "Connection: close\r\n"..
     "Content-Type: application/x-www-form-urlencoded\r\n"..
     "Content-Length: "..string.len(data).."\r\n"..
     "\r\n"..
     data
     --print(request)
     return request
end

-- This function registers a function to echo back any response from the server, to our DE1/NIOS system 
-- or hyper-terminal (depending on what the dongle is connected to)
function display(sck,response)
     --print(response)
end

-- When using send_sms: the "from" number HAS to be your twilio number.
-- If you have a free twilio account the "to" number HAS to be your twilio verified number.
function send_sms(from,to,body)

     data = {
      sid = TWILIO_ACCOUNT_SID,
      token = TWILIO_TOKEN,
      Body = string.gsub(body," ","+"),
      From = from,
      To = to
     }

     socket = net.createConnection(net.TCP,0)
     socket:on("receive",display)
     socket:connect(80,HOST)

     socket:on("connection",function(sck)
          post_request = build_post_request(HOST,URI,data)
          sck:send(post_request)
     end)
end

function database_twilio()
  ip = wifi.sta.getip()

 if(ip==nil) then
   --print("Connecting...")
 else
  tmr.stop(0)
  --print("Connected to AP!")
  send_sms("(778) 949-5030","(778) 999-9041",twilio)

 end
end

