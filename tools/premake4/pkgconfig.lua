function pkgConfig(opt, libs)
    local cmdStream = assert(io.popen("pkg-config --" .. opt .. " " .. table.concat(libs, " ")))
    local cmdOutput = assert(cmdStream:read("*l"))
    cmdStream:close()
    return cmdOutput;
end
