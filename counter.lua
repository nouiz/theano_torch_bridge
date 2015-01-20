package.path='/home/nouiz/torch/share/lua/5.1/?.lua;/home/nouiz/torch/share/lua/5.1/?/init.lua;./?.lua;/home/nouiz/torch/share/luajit-2.1.0-alpha/?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/?/init.lua'
package.cpath='/home/nouiz/torch/lib/lua/5.1/?.so;./?.so;/usr/local/lib/lua/5.1/?.so;/usr/local/lib/lua/5.1/loadall.so'
require 'torch'
require 'nn'

local M = {}
local M_mt = { __metatable = {}, __index = M }
 
function M:new(start)
    if self ~= M then
        return nil, "First argument must be self"
    end
 
    local o = setmetatable({}, M_mt)
    o._count = start
    return o
end
setmetatable(M, { __call = M.new })
 
function M:add(amount)
    self._count = self._count + amount
end
 
function M:subtract(amount)
    self._count = self._count - amount
end
 
function M:increment()
    self:add(1)
end
 
function M:decrement()
    self:subtract(1)
end
 
function M:getval()
    return self._count
end
 
function M_mt:__tostring()
    return string.format("%d", self._count)
end

function M.print_rand()
    local n = torch.randn(10)
    print(n)
    return n
end

function M.print_i(data)
    print(data)
end
 
--function M.init()
--    return nn.Linear(30,40)
--end

--function M.forward(m, i)
--input = torch.DoubleTensor(30):fill(1)
--return m:forward(input)
--m:backward(input, grad)
--end

return M