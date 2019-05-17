# Color table editor

## Installation

```shell
git clone https://github.com/satoshishimada/color_table_editor.git
cd color_table_editor
make
```

## Usage

### Make image list

```shell
ls image_dir/*.jpg | sort > image_list
```

Download a sample color table.  

```shell
wget https://github.com/SatoshiShimada/color_table_editor/files/3192322/x00.log https://github.com/SatoshiShimada/color_table_editor/files/3192323/x01.log https://github.com/SatoshiShimada/color_table_editor/files/3192324/x02.log https://github.com/SatoshiShimada/color_table_editor/files/3192325/x03.log; cat x00.log x01.log x02.log x03.log > color_table.cnf
```

Run application: `./color_table_editor`  

Load color table by press the 'L' key.

### Key binds

| Key | Function |
| :-: | :-: |
| ESC | Exit |
| l | Load color table |
| s | Save color table |
| r | Reset color table |
| n | Next image |
| p | Previous image |
| 0 - 9 | Color margin |
| g | Select label green |
| w | Select label white |
| b | Select label black |

## License

MIT License

## Author

Satoshi Shimada

