{
    "targets": [
        {
            "target_name": "xfyun",
            "sources": ["src/xfyun.cc"],
            "conditions":[
            ['OS=="linux"',{
                "libraries": ['<(module_root_dir)/src/sdk/linux/libs/<!(node -e "console.log(process.arch)")/msc.lib']
            }],
            ['OS=="win"',{
                "libraries": ['<(module_root_dir)/src/sdk/<!(node -e "console.log(process.platform)")/libs/<!(node -e "console.log(process.arch)")/msc.lib']
            }]
            ],
            "include_dirs": [
                "./src/sdk/include",
                "<!(node -e \"require('nan')\")"
            ],
            "cflags": ["-g", "-Wall", "-Isrc/sdk/include"]
        }
    ]
}
