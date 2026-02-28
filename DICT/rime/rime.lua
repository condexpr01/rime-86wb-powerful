---@diagnostic disable: undefined-global, lowercase-global, unused-local

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
	if (input == "zxxx") then
	end

end
]]

