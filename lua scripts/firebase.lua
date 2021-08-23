-- this is the web address of the relay web site that our dongle sends the initial HTTP request to
HOST = "shopal391.firebaseio.com" 

-- following defines print entire list
URI = "/shopal_list.json"

-- run garbage collecton
collectgarbage()


-- Configure as simple http client
function build_get_request(host, uri)
	--request = "c"
    request = "GET "..uri.." HTTP/1.1\r\n"..
            "Host: "..host.."\r\n"..
            "Connection: close\r\n"..
			--"Content-Type:application/json\r\n"..
			--"Content-Type: application/x-www-form-urlencoded\r\n"..
            --"Accept-Charset: utf-8\r\n"..
            --"Accept-Encoding: \r\n"..
            --"User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n".. 
            "Accept: */*\r\n\r\n"
     --print(request)
     return request
end


function download_database(path, callback)
    -- open a file on the WiFi dongle flash file system. File is created for writing
    file.remove(path)
    file.open(path, "w+")
   
    -- flag for if we fail somehow to retrieve db
    --payloadFound = false

    --create a connection to the website here
    local socket = tls.createConnection(net.TCP, 0)
    -- socket:on("receive", writefile)
    socket:on("receive", function(sck, payload)		-- second parameter is the received data as string
	-- optional next line shows the packets of data arriving from the web site
		--print("Downloading Database , Writing to File ...")
		--print(payload)
		--file.write(payload)
		file.write(string.sub(payload, string.find(payload, "\r\n\r\n") + 4) ) 
		-- file.flush()
		file.flush()
		file.close()
		--collectgarbage()
        end) 

    -- -- this call back function is called when we disconnect from the web site
    socket:on("disconnection", function(sck) 		-- the second parameter is error code
        sck = nil
        --file.close()
        callback("ok")
    end)		

	-- this call back function is called when a connection to the web site is made
   -- connect to the web site here
	socket:on("connection",function(sck)
          get_request = build_get_request(HOST,URI)
          sck:send(get_request)
     end)
	
    socket:connect(443,HOST)	-- NOT port 80 which is for HTTP, port 443 is for secure HTTP
	
end	
	 


function get_db()
	-- run garbage collecton
    --collectgarbage()
	ip = wifi.sta.getip()
	count = 0;
	
	if (ip == nil) then 
	    --print ("Connecting...")
	else
	    tmr.stop(0)
		download_database("database.json", function (s) end) -- parameters: path, callback
		--print("Database downloaded!")
	end
	
end



function make_db()
	-- run garbage collecton
	--collectgarbage()
	content = ""
	db = ""
	fd = file.open("database.json", "r")
	if (fd) then
		--print(file.read('\n'))
		content = file.read('\n')
		fd.close()
		fd = nil
	end
	-- format string
	for w in string.gmatch (content, "\"name\":\"([%w%s]+)\"" ) do	
		db  = db ..",".. w 
	end
	
end


function print_db()
		print(db)
end


function rm_db()
	file.remove("database.json")
end


