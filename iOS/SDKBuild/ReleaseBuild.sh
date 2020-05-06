cd `dirname $0`

if [ -f ./Release-iphoneos/QMediaSDK.framework/QMediaSDK ] && [ -f ./Release-iphonesimulator/QMediaSDK.framework/QMediaSDK ]; then
cp -r -f ./Release-iphoneos/QMediaSDK.framework ./../Product/SDK/
lipo -create ./Release-iphoneos/QMediaSDK.framework/QMediaSDK ./Release-iphonesimulator/QMediaSDK.framework/QMediaSDK -output ./../Product/SDK/QMediaSDK.framework/QMediaSDK
fi

if [ -f ./Debug-iphoneos/QMediaSDK.framework/QMediaSDK ] && [ -f ./Debug-iphonesimulator/QMediaSDK.framework/QMediaSDK ]; then
cp -r -f ./Debug-iphoneos/QMediaSDK.framework ./Debug-Product/
lipo -create ./Debug-iphoneos/QMediaSDK.framework/QMediaSDK ./Debug-iphonesimulator/QMediaSDK.framework/QMediaSDK -output ./Debug-Product/QMediaSDK.framework/QMediaSDK
fi
