---@diagnostic disable: undefined-global, lowercase-global

-- 时间
time = function (input, seg)
	if (input == "time") then
		yield(Candidate("time", seg.start, seg._end , os.date("%Y-%m-%d %H:%M:%S"), "time"))
	end
end

-- pieces
-- 预期添加z为前缀的编码：
-- 预期提供代码片，快捷用语，etc.

--[[
pieces = function (input, seg)

	--快捷编辑
	if (input == "znv") then
		os.execute(string.format("%s %s &",
			"neovide --maximized --title-hidden",
			"~/.local/share/fcitx5/rime/powerful86wubi.dict.yaml")
		)
	end

end
]]
