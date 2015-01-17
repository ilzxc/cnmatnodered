cd CNMAT_2_oexpr/
node-gyp rebuild
cp -r build ../../02__o.NODERED_release/node_modules/o_expr/
cd ..
cd CNMAT_3_oparse/
node-gyp rebuild
cp -r build ../../02__o.NODERED_release/node_modules/o_parse/
cd ../CNMAT_6_flattenexplode/
node-gyp rebuild
cp -r build ../../02__o.NODERED_release/node_modules/o_flattenexplode/