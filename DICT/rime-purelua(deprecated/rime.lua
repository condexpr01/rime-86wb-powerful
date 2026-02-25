---@diagnostic disable: lowercase-global, unused-local, undefined-global, assign-type-mismatch

power86dict = {} --码表
power86func = {} --函表

--全局日志
_,log = pcall(require,"log")
if not _ then log = nil end

-- 1:半角,0全角,2或其它为系统符号
is_half = 1

-- ##################
-- # 是rime也是脚本 #
-- ##################
-- 用在脚本中不要用rime api


------------ rime ---------------

--###################
--#    debug        #
--###################
-- 重定向 print 到日志文件
-- tmux终端用tail -n 10 -f log.lua分屏journalctl --user -f
-- 不仅平台上好找，内置命令模式也可以require("log")访问

-- 我会在下面给留一些print的注释以快捷使用

print = function (...)

	--需要require'log'返回的是张表
	if not type(log) == "table" then return end


	local args = {...}

	--硬编码,达到长度1M个成员,自动清除(0xfffff==1024*1024-1)
	if (#log >= 0xfffff) then log = {} end

	--追加日志
	for n=1,#args do
		log[#log+1] = tostring(args[n])
			:gsub('\\', '\\\\'):gsub('"', '\\"')
			:gsub('\n', '\\n'):gsub('\r', '\\r')
			:gsub('\t', '\\t'):gsub('\0', '\\0')
	end

	--定位
	local logpath = package.searchpath("log",package.path)

	if not logpath then
		logpath = package.searchpath("lua.log",package.path)
		if not logpath then return end
	end

	local logfile = io.open(logpath,"w")
	if not logfile then return end

	--回写日志开始
	logfile:write("return {\n")

	for _,v in ipairs(log) do
		logfile:write(string.format('"%s",\n',v))
	end

	logfile:write("}")
	logfile:close()
end




--###################
--#   processor     #
--###################
--[[
	arg:[key_event]

	func_return:
		KReject = 0 输入法不处理,给系统
		KAccept = 1 拦截不给后面处理器
		KNoop   = 2 会给后面处理器


	print(string.format("%s:%#8.8x[%s]",
		os.date("%Y-%m-%d %H:%M:%S"),
		key_event.keycode,key_event:repr()))

	print("key_event.modifier",key_event.modifier) -- (ctrl alt shift ...) bitwise or
	print("key_event:shift",  key_event:shift())
	print("key_event:ctrl",   key_event:ctrl())
	print("key_event:alt",    key_event:alt())
	print("key_event:release",key_event:release())

]]

--[[
	arg:[env]

	env.engine.context
	env.engine:commit_text("condexpr01")

	env.engine.context:push_input(string)
	env.engine.context:pop_input(number)
	print("env.engine.context.input",env.engine.context.input)
	
]]

-- ascii_lower and backspace and enter and escape
-- 还有顶字上屏
power86_processor= {
	init = function (env) end,

	-- env.engine env.namespace
	func = function (key_event,env)
		local KReject = 0
		local KAccept = 1
		local KNoop   = 2

		-- #################
		-- #push into input#
		-- #################
		-- [a-z],ascii[0x61-0x7a]
		if( (key_event.keycode >= 0x61 and key_event.keycode <= 0x7a)
			and key_event.modifier == 0
			and key_event:release() == false
		) then

			-- #################
			-- #  顶功-顶选    #
			-- #################
			-- 字母顶字母
			-- 条件:alphabet下,有候选,上下文前缀>=4,顶它
			if (env.engine.context:has_menu() == true
				and env.engine.context.input:len() >= 4)
			then

				env.engine.context:select(0)
				env.engine.context:commit()

			end

			-- #################
			-- #  顶功-顶符    #
			-- #################
			-- 字母顶字符

			if (env.engine.context.input:len() >= 1) then

				--字符只会在input:byte(1),或末尾,以顶的规则
				--这里是顶字符所以是input:byte(1),末尾是被字符顶
				local input_head = env.engine.context.input:byte(1)

				-- visiable sign acsii[0x20-0x7e]与[0x30-0x39],[0x40,0x5a],[0x61,0x7a],[0x7e]差集
				if  (input_head >= 0x20 and input_head <= 0x7e) and

					not((input_head >= 0x30 and input_head <= 0x39)
					or(input_head >= 0x40 and input_head <= 0x5a)
					or(input_head >= 0x61 and input_head <= 0x7a)
					or(input_head == 0x7e))
				then

					--有候选的字符
					if (env.engine.context:has_menu() == true
						and env.engine.context.input:len() >= 1)
					then

						env.engine.context:select(0)
						env.engine.context:commit()

					--无候选的字符(命令模式用了z set('sys')时)
					elseif (env.engine.context.input:len() >= 1) then
						env.engine:commit_text(env.engine.context.input)
						env.engine.context:clear()
					end

				end
			end

			-- ###############################
			env.engine.context:push_input(string.char(key_event.keycode))

			return KAccept
		end

		-- #################
		-- #  backspace    #
		-- #################
		-- backspace acsii[0x08] [0xff08]
		-- pop 1
		if( (key_event.keycode == 0x08
			or key_event.keycode == 0xff08
			or key_event.repr == "BackSpace")
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context.caret_pos > 0
		) then

			env.engine.context:pop_input(1)

			return KAccept
		end

		-- #################
		-- #    esc        #
		-- #################
		-- escape acsii[0x1b] [0xff1b]
		-- pop all
		if( (key_event.keycode == 0x1b
			or key_event.keycode == 0xff1b
			or key_event.repr == "Escape")
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context.caret_pos > 0
		) then

			env.engine.context:clear()

			return KAccept
		end

		-- #################
		-- #   enter       #
		-- #################
		-- enter acsii /r[0x0a] [0xff0a] /n[0x0d] [0xff0d] 
		-- pop all and commit
		if( ((key_event.keycode == 0x0d or key_event.keycode == 0xff0d)
			or (key_event.keycode == 0x0a or key_event.keycode == 0xff0a)
			or key_event.repr == "Return")
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context.caret_pos > 0
		) then

			env.engine:commit_text(env.engine.context.input)
			env.engine.context:clear()

			return KAccept
		end

		return KNoop
	end,

	fini = function (env) end,
}

-- z命令模式下enter,加载lua代码
local function luacmd_enter(luacmd,key_event,env)
	--[[
		print("version",_G._VERSION) --lua5.4
		print("is_load_ok",type(load)) --ok
		print("is_loadstring_ok",type(loadstring)) --nil

		-- 5.4 load(chunk,chunkname,mode,env)
	]]


	--################
	--# luaoutput in #
	--################
	local luaoutput = {} -- luacmd的print用的表
	local function print_to_luaoutput(...)
		for i = 1, select("#", ...) do
			luaoutput[#luaoutput+1] = select(i, ...)
		end
	end

	--########
	--# load #
	--########
	local luastatus --luacmd的返回状态

	local luafunc,luaerr = load(luacmd,"luacmd","bt",
		setmetatable(
			--env
			{
				print=print_to_luaoutput,

				--符号或状态设置,半角是默认的
				--e.g. z set('full')
				--e.g. z set('sys')
				set=function (type)
					if (type == "half") then is_half = 0 end
					if (type == "full") then is_half = 1 end
					if (type == "sys") then is_half = 2 end

				end,

				--dict别名的d:
				--e.g. z return d['wv2']   会上屏你好
				--e.g. z return d['wqvb1'] 会上屏你好
				d = setmetatable({},
					{
						__index=function(_,k)
							return power86dict[k] and power86dict[k][1]
						end,
						__newindex=function(_,k,v1)
							local t=power86dict[k] or {}
							t[1]=v1
							t[2]=0
							power86dict[k]=t
						end
					}
				),

				--input别名i:用表模拟输入法输入给输入法
				--e.g. return i('puv1 ujf1 fi1 go1') 会上屏"初音未来"
				--e.g. d["miku1"]=i('puv1 ujf1 fi1 go1') 会增改码表,使输入miku时1选为"初音未来"
				i = function(input)
					local cast = ""
					for codec in input:gmatch("%S+") do
						--不加数字默认找1选
						if (codec:sub(-1):match("%d") == nil) then codec = codec .. '1' end

						cast = cast .. ((power86dict[codec] and power86dict[codec][1]) or "")
					end

					return cast
				end,

				-- update dict
				w = function (should_sort)
					local luadict,file,filepath

					luadict = power86dict
					filepath = package.searchpath("power86dict",package.path)
					if not filepath then
						filepath = package.searchpath("lua.power86dict",package.path)
						if not filepath then
							print_to_luaoutput("[write error]Dict not found.")
							return
						end
					end

					print_to_luaoutput(filepath)

					file = io.open(filepath,"w")
					if not file then
						print_to_luaoutput("[write error]Dict not opened.")
						return
					end

					-- 码表开头
					file:write("return {\n")

					--不给参数之类的，用哈希序，直接写更快
					if (not should_sort) then
						for k, v in pairs(luadict) do
							if type(v) == "table" then
								file:write(string.format('["%s"]={"%s","%s","%s"},\n',
									k, v[1], v[2],v[3]))
							else
								print_to_luaoutput(k, " wrong format or value type:", type(v) ,'\n')
							end
						end
					end

					-- 排序(混合快排应该nlogn)
					if (should_sort) then
						local keys = {}
						for k in pairs(luadict) do keys[#keys+1] = k end

						table.sort(keys,function (a, b)
							-- 尾数字：从末尾连续数字
							local na = a:match("%d+$")  or "1"
							local nb = b:match("%d+$")  or "1"

							local num_a, num_b = tonumber(na), tonumber(nb)

							-- 字母段：去掉尾数字剩下的
							local la = a:gsub("%d+$","")
							local lb = b:gsub("%d+$","")

							-- 长度优先 a<aa<aaa
							if #la ~= #lb then return #la < #lb end

							-- 相同长度再字典序
							if la ~= lb   then return la  < lb  end

							-- 序号 
							return num_a < num_b
						end)

						-- 顺序写
						for _, k in ipairs(keys) do
							local v = luadict[k]
							if type(v) == "table" then
								file:write(string.format('["%s"]={"%s","%s","%s"},\n',
									k, v[1], v[2],v[3]))
							else
								print_to_luaoutput(k, " wrong format or value type:", type(v))
							end
						end

					end

					-- 码表结尾
					file:write("}\n")
					file:close()

				end,

			},

			--mt
			{
				--找全局环境
				--这意味着词典在环境
				--power86dict,
				--power86func,
				__index = _G,
			}
		))

	if luafunc then
		luastatus = table.pack(pcall(luafunc))

		--error
		if(not luastatus[1]) then
			print_to_luaoutput(luastatus[2])
		end

		--return
		print_to_luaoutput(table.unpack(luastatus,2,luastatus.n))

	else
		print_to_luaoutput(luaerr)
	end
	--#############################################


	--#################
	--# luaoutput out #
	--#################
	-- 上屏luaoutput
	for _,v in pairs(luaoutput) do
		env.engine:commit_text(tostring(v))
	end

end

power86_luacmd= {

	-- ###################
	-- #   luacmd init   #
	-- ###################
	init = function (env)
		last_luacmd = "" --global

	end,


	-- env.engine env.namespace
	func = function (key_event,env)
		local KReject = 0
		local KAccept = 1
		local KNoop   = 2

		print(string.format("%s:%#8.8x[%s]",
			os.date("%Y-%m-%d %H:%M:%S"),
			key_event.keycode,key_event:repr()))

		input = env.engine.context.input


		if ( input:len() >= 1 and string.sub(input,1,1) == 'z' ) then

			-- ####################
			-- #  cmd input       #
			-- ####################
			-- visiable acsii[0x20,0x7e]
			if( (key_event.keycode >= 0x20 and key_event.keycode <= 0x7e)
				and key_event:release() == false
			) then

				env.engine.context:push_input(string.char(key_event.keycode))

			end

			-- #################
			-- # cmd backspace #
			-- #################
			-- backspace acsii[0x08] [0xff08]
			-- pop 1
			if( (key_event.keycode == 0x08
				or key_event.keycode == 0xff08
				or key_event.repr == "BackSpace")
				and key_event.modifier == 0
				and key_event:release() == false
			) then

				env.engine.context:pop_input(1)

			end

			-- #################
			-- #   cmd esc     #
			-- #################
			-- escape acsii[0x1b] [0xff1b]
			-- pop all
			if( (key_event.keycode == 0x1b
				or key_event.keycode == 0xff1b
				or key_event.repr == "Escape")
				and key_event.modifier == 0
				and key_event:release() == false
			) then

				env.engine.context:pop_input(input:len())
				env.engine.context:commit()

			end

			-- #################
			-- #   cmd enter   #
			-- #################
			-- enter acsii /r[0x0a] [0xff0a] /n[0x0d] [0xff0d] 
			-- run luacmd
			if( ((key_event.keycode == 0x0d or key_event.keycode == 0xff0d)
				or (key_event.keycode == 0x0a or key_event.keycode == 0xff0a)
				or key_event.repr == "Return")
				and key_event.modifier == 0
				and key_event:release() == false
				and env.engine.context.caret_pos > 0
			) then

				--################

				local luacmd=input:sub(2)

				--zz执行前一次的,否则就是现在的
				if (input:sub(1,2) == "zz"
					and input:len() == 2)
				then
					pcall(luacmd_enter,last_luacmd,key_event,env)
				else
					pcall(luacmd_enter,luacmd,key_event,env)
					last_luacmd = luacmd
				end

				--################
				env.engine.context:pop_input(input:len())
				env.engine.context:commit()

			end

			--print("#luacmd命令模式","[".. input:sub(2) .."]")

			--拦截命令模式所有键,内部处理
			return KAccept

		end

		return KNoop

	end,

	-- ###################
	-- #   luacmd fini   #
	-- ###################
	fini = function (env)
		last_luacmd = nil
	end,
}











--[[
	print("has menu",env.engine.context:has_menu())
	env.engine.context:select(index) -- put into preedit bar
	env.engine:commit()  -- commit selected candidate
]]
power86_selector= {
	init = function (env) end,

	-- env.engine env.namespace
	func = function (key_event,env)
		local KReject = 0
		local KAccept = 1
		local KNoop   = 2


		--###################
		--# number selector #
		--###################
		-- numbers acsii[0x30-0x39]
		if( (key_event.keycode >= 0x30 and key_event.keycode <= 0x39)
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context.caret_pos > 0
			and env.engine.context:has_menu() == true
		) then

			-- index 0-9 -> keycode 0x30-0x39
			if (key_event.keycode ~= 0x39)then
				env.engine.context:select(key_event.keycode - 0x30 - 1)
			else
				env.engine.context:select(10 - 1)
			end

			env.engine.context:commit()

			return KAccept
		end

		--##################
		--# space selector #
		--##################
		-- space acsii[0x20]
		if( key_event.keycode == 0x20
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context.caret_pos > 0
		) then

			--env.engine.context:select(0)
			if ( env.engine.context:has_menu() == true) then
				env.engine.context:confirm_current_selection()
			end

			env.engine.context:commit()

			return KAccept

		end

		--######################
		--# semicolon selector #
		--######################
		-- semicolon acsii[0x3b]
		if( key_event.keycode == 0x3b
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context:has_menu() == true
		) then

			env.engine.context:select(1)

			env.engine.context:commit()

			return KAccept
		end

		--########################
		--# singlequote selector #
		--########################
		-- singlequote acsii[0x27]
		if( key_event.keycode == 0x27
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context:has_menu() == true
		) then

			env.engine.context:select(2)

			env.engine.context:commit()

			return KAccept
		end

		--########################
		--#    left bracket      #
		--########################
		-- left bracket acsii[0x5b]
		if( key_event.keycode == 0x5b
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context:has_menu() == true
		) then

			-- page up
			-- 构造KeyEvent(keycode,modifier)
			env.engine:process_key(KeyEvent(0xff9a,0))

			return KAccept
		end

		--########################
		--#    right bracket     #
		--########################
		-- left bracket acsii[0x5d]
		if( key_event.keycode == 0x5d
			and key_event.modifier == 0
			and key_event:release() == false
			and env.engine.context:has_menu() == true
		) then

			-- page up
			-- 构造KeyEvent(keycode,modifier)
			env.engine:process_key(KeyEvent(0xff9b,0))

			return KAccept
		end


		return KNoop
	end,

	fini = function (env) end,
}


power86_sign = {
	init = function (env) end,

	-- env.engine env.namespace
	func = function (key_event,env)
		local KReject = 0
		local KAccept = 1
		local KNoop   = 2

		--###################
		--# push_input sign #
		--###################
		-- visiable sign acsii[0x20-0x7e]与[0x30-0x39],[0x40,0x5a],[0x61,0x7a],[0x7e]差集
		if( (key_event.keycode >= 0x20 and key_event.keycode <= 0x7e)
			and key_event:release() == false
		) then

			-- #################
			-- #   字符-顶功   #
			-- #################
			-- 字符顶字符或字母
			-- 条件:字符下,有候选,上下文前缀>=1,顶它
			if (env.engine.context:has_menu() == true
				and env.engine.context.input:len() >= 1)
			then

				env.engine.context:select(0)
				env.engine.context:commit()

			end

			-- 条件:字符下,上下文前缀>=1,顶编码
			if (env.engine.context.input:len() >= 1)
			then
				env.engine:commit_text(env.engine.context.input)
				env.engine.context:pop_input(env.engine.context.input:len())
			end


			--不处理的
			if (key_event.keycode >= 0x30 and key_event.keycode <= 0x39)
				or(key_event.keycode >= 0x40 and key_event.keycode <= 0x5a)
				or(key_event.keycode >= 0x61 and key_event.keycode <= 0x7a)
				or(key_event.keycode == 0x7e)
			then
				return KNoop
			end

			--只接受ascii字符
			env.engine.context:push_input(string.char(key_event.keycode))

			return KAccept
		end

		return KNoop
	end,

	fini = function (env) end,
}




--###################
--#   segmentor     #
--###################

--[[
	func_ret:
		true: 继续后续segmentor处理
		false: 停止后续segmentor处理
]]

--[[
	arg:[segmentation]

	segmentation.input

	--  +: Set{'a', 'b'} + Set{'b', 'c'} return Set{'a', 'b', 'c'}
	--  -: Set{'a', 'b'} - Set{'b', 'c'} return Set{'a'}
	--  *: Set{'a', 'b'} * Set{'b', 'c'} return Set{'b'}
	segmentation.tag -- Set

	segmentation.prompt -- string
	print(segmentation:get_confirmed_position())
	print(input,segmentation:get_confirmed_position())
]]

--[[
	arg:[env]

	env.engine.context

]]


power86_segmentor= {
	init = function (env) end,

	func = function (segmentation, env)

		if env.engine.context.caret_pos == 0 then
			return false
		end

		local seg=Segment(segmentation:get_confirmed_position(),env.engine.context.caret_pos)

		--sparkles✨
		--tada🎉
		seg.prompt = "🎉"
		seg.tag = Set({"power86"})

		segmentation:add_segment(seg)


		return true
	end,

	fini = function (env)  end
}










--###################
--#   translator    #
--###################

power86_translator = {

	init = function (env)

		local ok,table

		-- 加载码表，两种路径都测试
		ok,table= pcall(require,"power86dict")
		if (not ok) then
			ok,table= pcall(require,"lua.power86dict")

			if (not ok) then
				--[[print(string.format(
					"error_loading_power86dict: %s",
					power86dict))]]

				return
			end
		end

		power86dict = table

		-- 加载函表，两种路径都测试
		ok,table= pcall(require,"power86func")
		if (not ok) then
			ok,table= pcall(require,"lua.power86func")

			if (not ok) then
				--[[print(string.format(
					"error_loading_power86func: %s",
					power86func))]]

				return
			end
		end

		power86func = table
	end,

	-- no return, void function
	func = function (input,segment,env)

		local index
		local codec

		local sign
		local signtable


		-- 1.查函表
		index=1
		while(true) do
			codec = string.format("%s%d",input,index);

			if ( power86func[codec] ~= nil) then

				local func_ret = power86func[codec]()

				yield(Candidate("power86func",segment.start,segment._end,
					func_ret[1],
					string.format("〔%d,%s✨func〕",index,func_ret[2])))

				index=index+1;
			else break end

		end


		-- 2.查码表
		index=1
		while(true) do
			codec = string.format("%s%d",input,index);

			if ( power86dict[codec] ~= nil) then
				yield(Candidate("power86dict",segment.start,segment._end,
					power86dict[codec][1],
					string.format("〔%d,%s,%s✨〕",index,power86dict[codec][2],power86dict[codec][3])))

				index=index+1;
			else break end

		end



		-- 3.查函表中的符表
		codec = input:sub(-1) or ""
		sign = nil
		signtable = nil

		--半角
		if (is_half == 1) then
			_,sign=pcall(power86func["half"])
		end

		--全角
		if (is_half == 0) then
			_,sign=pcall(power86func["full"])
		end

		--防止命令模式解析符号
		if (input:len() >= 1 and input:sub(1,1) == 'z') then
			signtable = nil
		elseif (sign ~= nil) then
			signtable = sign[1] and sign[1][codec]
		end


		--符号只有首选那么提交它
		if (signtable ~= nil and signtable[2] == nil and sign ~= nil)

			--命令模式z set'sys' 后符号也认为是首选
			-- visiable sign acsii[0x20-0x7e]与[0x30-0x39],[0x40,0x5a],[0x61,0x7a],[0x7e]差集
			and ((codec:byte(1) >= 0x20 and codec:byte(1) <= 0x7e) and

			not((codec:byte(1) >= 0x30 and codec:byte(1) <= 0x39)
			or(codec:byte(1) >= 0x40 and codec:byte(1) <= 0x5a)
			or(codec:byte(1) >= 0x61 and codec:byte(1) <= 0x7a)
			or(codec:byte(1) == 0x7e)))
		then

			-- menu default select
			if (env.engine.context:has_menu() == true) then
				env.engine.context:confirm_current_selection()
				env.engine.context:commit()
			end

			--全半角表
			if (signtable ~= nil) then
				env.engine:commit_text(tostring(signtable[1]))
				env.engine.context:pop_input(env.engine.context.input:len())

			--系统的ascii,is_half=2
			elseif is_half ~= 1 and is_half ~= 0 then
				env.engine:commit_text(tostring(codec))
				env.engine.context:pop_input(env.engine.context.input:len())
			end


		--符号不只有首选,推出候选
		elseif (signtable ~= nil and sign ~= nil) then

			for k,v in ipairs(signtable) do
				yield(Candidate("power86sign",segment.start,segment._end,
					v,string.format("〔%d🪪,%s✨〕",k,sign[2])))
			end


		end


	end,


	fini = function (env)
		--[[print("#fini triggered!#")]]
	end,

}










--###################
--#   filter        #
--###################

--[[
	args:[translation] 候选流
		translation:iter() 迭代器
]]
-- void function
power86_filter = {
	init = function(env) end,

	tags_match = function(segment, env)

		-- true :match
		-- false:unmatch
		return true
	end,

	func = function(translation, env)

		for cand in translation:iter() do
			yield(cand)
		end

	end,

	fini = function(env) end,
}


------------ rime END ---------------



