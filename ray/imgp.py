# generates imgs.h and imgn.h from images
from os import walk
from datetime import datetime
from re import findall

path = 'images'
try:
    _, _, filenames = next ( walk ( path + "/." ) )
except StopIteration:
    print ( "path " + path + " cannot be iterated" )
    exit ( -2 )
bmps = [ f for f in filenames if f[ -4: ] == ".bmp" ]
time = datetime.now().strftime ( "%Y-%m-%d %H:%M:%S" )
# names
names_file = "imgs.h"
with open ( names_file, "w", encoding="utf-8" ) as f:
    header = "IMGS_H"
    f.write ( "// This file generated by imgp at " + time + "\n" )
    f.write ( "#ifndef " + header + "\n" )
    f.write ( "#define " + header + "\n" )
    f.write ( "\n" )
    f.write ( "#define IMGS_LENGTH " + str ( len ( bmps ) ) + "\n" )
    f.write ( "const char *imgs[] = {" + "\n" )
    for bmp in bmps: f.write( "  \"" + path + "/" + bmp + "\",\n" )
    f.write ( "};\n" )
    f.write ( "\n" )
    f.write ( "#endif // " + header + "\n" )
print ( "created " + names_file )

# if file name contains digit then it is array
# so i put all indices in arrays, most of them
# are will contain one element, others more.
# bmpindex is incremented because of not initialized
# values will be treated as 0. this is need to
# calculate array length for texture.
# indices_max is constant length of each array.
indices = {}
indices_max = 1
for bmpindex, bmp in enumerate ( bmps ):
    s = bmp[ :-4 ]
    r = findall ( "[0-9]+", s )
    if len ( r ) > 0:
        s = s[ :-len ( r[ -1 ] ) ]
    if not ( s in indices ):
        indices[ s ] = [ None ]
    if len ( r ) > 0:
        i = int ( r[ -1 ] ) - 1
        try:
            indices[ s ][ i ] = bmpindex + 1
        except IndexError:
            for _ in range ( i - len ( indices[ s ] ) + 1 ):
                indices[ s ].append ( None )
            indices[ s ][ i ] = bmpindex + 1
            indices_max = i + 1
    else:
        indices[ s ][ 0 ] = bmpindex + 1
# if any array contain None then indices in file name are wrong
for key in indices:
    if None in indices[ key ]:
        print ( "array file " + key + "[?] has wrong indices" )
        exit ( -3 )

#TODO: type of texture ( surface, object, interface )

# # defines
defines_file = "imgn.h"
with open ( defines_file, "w", encoding="utf-8" ) as f:
    header = "IMGN_H"
    f.write ( "// This file generated by imgp at " + time + "\n" )
    f.write ( "#ifndef " + header + "\n" )
    f.write ( "#define " + header + "\n" )
    f.write ( "\n" )
    f.write ( "#define IMGN_LENGTH " + str ( len ( indices ) ) + "\n" )
    f.write ( "#define IMGN_ARR_LENGTH " + str ( indices_max ) + "\n" )
    f.write ( "const unsigned int imgn[ IMGN_LENGTH ][ IMGN_ARR_LENGTH ] = {" )
    for key in indices:
        f.write ( "{" )
        for index in indices[ key ]:
            f.write ( str ( index ) + "," )
        f.write ( "}," )
    f.write ( "};\n" )
    f.write ( "\n" )
    f.write ( "#endif // " + header + "\n" )
print ( "created " + defines_file )
