#!/usr/bin/env bash
GITURL="https://gitlab.com/CalcProgrammer1/OpenRGB/-"
GITPARAM="?inline=false"
WEBSITE="www.openrgb.org"
ICONFILE="qt/OpenRGB.ico"
LICENSEFILE="scripts/License.rtf"
ZIP="/jobs/artifacts/master/download?job=Windows+64"

PRODUCTID=$(uuidgen -n @url -N ${WEBSITE} --sha1 | awk '{ print toupper($0) }')
PRODUCTNAME="OpenRGB"
VENDOR=$PRODUCTNAME
PRODUCTCOMMENT="Open source RGB lighting control that doesn't depend on manufacturer software."

MAJOR=$(grep "MAJOR\ *=" OpenRGB.pro | cut -d= -f 2 | tr -d [:space:])
MINOR=$(grep "MINOR\ *=" OpenRGB.pro | cut -d= -f 2 | tr -d [:space:])
REVISION=$(grep "REVISION\ *=" OpenRGB.pro | cut -d= -f 2 | tr -d [:space:])
#The commit count since last tag. The commit ID can be added with group \3 however Wix only accepts integers
RELEASE=$(git describe --tags --abbrev=8 | sed -E "s/(.*)-([0-9]*)-g(\w*)/\2/g")

VERSION="${MAJOR}.${MINOR}.${REVISION}.${RELEASE}"

XMLOUTFILE=${PRODUCTNAME,,}".wxs"

echo -e "Zip URL:\t" $GITURL$ZIP
echo -e "Icon URL:\t" $GITURL$ICONFILE
echo -e "License URL:\t" $GITURL$LICENSEFILE

echo -e "Product UUID:\t" $PRODUCTID
echo -e "Product:\t" $PRODUCTNAME
echo -e "Vendor:\t\t" $VENDOR
echo -e "Version:\t" $VERSION

ICONID=$(basename "$ICONFILE")
#echo -e "\n\nGetting ${PRODUCTNAME}'s icon from repo:\t"$ICONID
#wget -O ${ICONID} "${GITURL}${ICONFILE}${GITPARAM}"

LICENSEID=$(basename "$LICENSEFILE")
#echo -e "\nGetting ${PRODUCTNAME}'s icon from repo:\t"$LICENSEID
#wget -O ${LICENSEID} "${GITURL}${LICENSEFILE}${GITPARAM}"

#echo -e "\nProcessing latest Windows 64 package from " $GITURL
#ZIPFILE=${PRODUCTNAME,,}".zip";
#echo -e "Zipfile:\t${ZIPFILE}\nZip:\t\t${ZIP}"
#wget -O ${ZIPFILE} "${GITURL}${ZIP}"
#unzip ${ZIPFILE}

#Wix and / or Wine have issues with the mixed upper and lower case letters
WORKING_PATH="orgb/"
ZIP_PATH="OpenRGB Windows 64-bit/"
mv -T "${ZIP_PATH}" ${WORKING_PATH}

EXTENSION="orp"
SAVE_FILE="${PRODUCTNAME}.${EXTENSION}"
count=1

for file in "$WORKING_PATH"/*;
do
    filename=$(basename "$file")
    if [ $filename == "${PRODUCTNAME}.exe" ] ; then
        #If this is the executable treat as a special case as we need the reference for later
        EXE_ID=${PRODUCTNAME}00
        EXE_FILE=${filename}
        #Add special entry to files list
        FILES="$FILES\t\t\t\t\t<File Id='${EXE_ID}' Source='${WORKING_PATH}${filename}'/>\n"
    elif [ -d "$file" ] ; then
            #If this is a directory then we need to add another component
            COMPONENTS="${COMPONENTS}\t\t\t<ComponentRef Id='${filename}Files'/>\n"
            TEMP="\t\t\t\t<Directory Id='${filename}' Name='${filename}'>\n\t\t\t\t\t<Component Id='${filename}Files' Guid='"$(uuidgen -t | awk '{ print toupper($0) }')"'>\n"
            for file2 in "$file"/*;
            do
                filename2=$(basename "$file2")
                TEMP="$TEMP\t\t\t\t\t\t<File Id='${PRODUCTNAME}${count}' Source='${WORKING_PATH}${filename}/${filename2}'/>\n"
                count=$((count+1))
            done
            DIRECTORIES="$DIRECTORIES$TEMP\t\t\t\t\t</Component>\n\t\t\t\t</Directory>\n"
    else
            #Any other file to files list
            FILES="$FILES\t\t\t\t\t<File Id='${PRODUCTNAME}${count}' Source='${WORKING_PATH}${filename}'/>\n"
            count=$((count+1))
    fi
done

echo -e "Building XML:\t" $XMLOUTFILE

XML_PACKAGE="\t<Package Keywords='Installer' Description='${PRODUCTNAME} Installer'\n\t\tComments=\"${PRODUCTCOMMENT}\" Manufacturer='OpenRGB'\n\t\tInstallerVersion='200' Languages='1033' Compressed='yes' SummaryCodepage='1252' Platform='x64'/>\n"
XML_MEDIA="\t<Media Id='1' Cabinet='${PRODUCTNAME,,}.cab' EmbedCab='yes'/>\n"
XML_CONDITIONS="\t<Condition Message='This package supports Windows 64bit Only'>VersionNT64</Condition>\n"
XML_ACTION_RUNAS_ADMIN="\t<CustomAction Id='LaunchOpenRGBAdmin' FileKey='${EXE_ID}' ExeCommand='${EXE_FILE} --loglevel 1 --nodetect' Execute='deferred' Return='asyncNoWait' Impersonate='no'/>\n"
XML_ACTION_FIRSTRUN="\t<CustomAction Id='LaunchOpenRGB' FileKey='${EXE_ID}' ExeCommand='--gui --loglevel 6' Execute='immediate' Return='asyncNoWait' Impersonate='yes'/>\n"
XML_ICON="\t<Icon Id='${ICONID}' SourceFile='${ICONFILE}'/>\n"
XML_ACTIONS_EXECUTE="\t<InstallExecuteSequence>\n\t\t<Custom Action='LaunchOpenRGBAdmin' After='InstallFiles'>NOT Installed</Custom>\n\t\t<Custom Action='LaunchOpenRGB' After='InstallFinalize'>NOT Installed</Custom>\n\t</InstallExecuteSequence>\n"
XML_WIX_UI="\t<UIRef Id='WixUI_Minimal'/>\n\t<UIRef Id='WixUI_ErrorProgressText'/>\n\t<WixVariable Id='WixUILicenseRtf' Value='${LICENSEFILE}'/>\n"
XML_METADATA="$XML_PACKAGE $XML_MEDIA $XML_CONDITIONS $XML_ACTION_RUNAS_ADMIN $XML_ACTION_FIRSTRUN $XML_ICON $XML_ACTIONS_EXECUTE $XML_WIX_UI"

XML_STARTMENU="\t\t<Directory Id='ProgramMenuFolder' Name='Programs'>\n\t\t\t<Directory Id='ProgramMenuDir' Name='${PRODUCTNAME}'>\n\t\t\t\t<Component Id='ProgramMenuDir' Guid='"$(uuidgen -t | awk '{ print toupper($0) }')"'>\n\t\t\t\t\t<RemoveFolder Id='ProgramMenuDir' On='uninstall'/>\n\t\t\t\t\t<RegistryValue Root='HKCU' Key='Software\[Manufacturer]\[ProductName]' Type='string' Value='' KeyPath='yes'/>\n\t\t\t\t</Component>\n\t\t\t</Directory>\n\t\t</Directory>\n"
XML_SHORTCUT="\t\t\t\t\t<Shortcut Id='startmenu${PRODUCTNAME}' Directory='ProgramMenuDir' Name='${PRODUCTNAME}' Target='[#${EXE_ID}]' WorkingDirectory='INSTALLDIR' Icon='${ICONID}' IconIndex='0' Advertise='no'/>\n"
XML_ASSOCIATE_FILE="\t\t\t\t\t<ProgId Id='${SAVE_FILE}' Description='${PRODUCTNAME} Profile'>\n\t\t\t\t\t\t<Extension Id='${EXTENSION}' ContentType='application/${EXTENSION}'>\n\t\t\t\t\t\t<Verb Id='open' Command='Open' TargetFile='${EXE_ID}' Argument='-p \"%1\"' />\n\t\t\t\t\t\t</Extension>\n\t\t\t\t\t</ProgId>\n"
XML_DIRECTORIES="\t<Directory Id='TARGETDIR' Name='SourceDir'>\n\t\t<Directory Id='ProgramFiles64Folder'>\n\t\t\t<Directory Id='INSTALLDIR' Name='${PRODUCTNAME}'>\n\t\t\t\t<Component Id='${PRODUCTNAME}Files' Guid='"$(uuidgen -t | awk '{ print toupper($0) }')"'>\n$FILES\n$XML_SHORTCUT\n$XML_ASSOCIATE_FILE\t\t\t\t</Component>\n$DIRECTORIES\t\t\t</Directory>\n\t\t</Directory>\n$XML_STARTMENU\t</Directory>\n"

XML_COMPONENTS="\t<Feature Id='Complete' Title='${PRODUCTNAME}' Description='Install all ${PRODUCTNAME} files.' Display='expand' Level='1' ConfigurableDirectory='INSTALLDIR'>\n\t\t<Feature Id='${PRODUCTNAME}Complete' Title='${PRODUCTNAME}' Description='The complete package.' Level='1' AllowAdvertise='no' InstallDefault='local'>\n\t\t\t<ComponentRef Id='${PRODUCTNAME}Files'/>\n$COMPONENTS\t\t\t<ComponentRef Id='ProgramMenuDir'/>\n\t\t</Feature>\n\t</Feature>\n"
XML_DATA="$XML_DIRECTORIES $XML_COMPONENTS"

#Wipe out any previous XMLOUTFILE and add the header
XML_HEADER="<?xml version='1.0' encoding='windows-1252'?>\n<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'>\n"
XML_PRODUCT="\t<Product Name='${PRODUCTNAME}' Manufacturer='${VENDOR}'\n\t\tId='${PRODUCTID}'\n\t\tUpgradeCode='"$(uuidgen -t | awk '{ print toupper($0) }')"'\n\t\tLanguage='1033' Codepage='1252' Version='${VERSION}'>\n$XML_METADATA\n$XML_DATA\n\t</Product>\n</Wix>"

echo -e $XML_HEADER $XML_PRODUCT > $XMLOUTFILE
echo -e "\t...Done!\n\n"


#Once the XML file manifest is created create the package
candle -arch x64 ${PRODUCTNAME,,}.wxs
light -sval -ext WixUIExtension ${PRODUCTNAME,,}.wixobj
