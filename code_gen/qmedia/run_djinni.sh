#! /usr/bin/env bash
set -eu
shopt -s nullglob

# Locate the script file.  Cross symlinks if necessary.
loc="$0"
while [ -h "$loc" ]; do
    ls=`ls -ld "$loc"`
    link=`expr "$ls" : '.*-> \(.*\)$'`
    if expr "$link" : '/.*' > /dev/null; then
        loc="$link"  # Absolute link
    else
        loc="`dirname "$loc"`/$link"  # Relative link
    fi
done
base_dir=$(cd "`dirname "$loc"`" && pwd)

# setup djinni src path and mmc project path
qmedia_project_dirctory="$base_dir/../.."
djinni_src_dirctory="$base_dir/../djinni"

temp_out="$base_dir/djinni-output-temp"

in="$base_dir/main.djinni"

cpp_out="$qmedia_project_dirctory/generated-src/cpp"
jni_out="$qmedia_project_dirctory/generated-src/jni"
objc_out="$qmedia_project_dirctory/generated-src/objc"
java_out="$qmedia_project_dirctory/generated-src/java/com/qmedia/editor/generated"

java_package="com.qmedia.editor.generated"

gen_stamp="$temp_out/gen.stamp"

if [ $# -eq 0 ]; then
    # Normal build.
    true
elif [ $# -eq 1 ]; then
    command="$1"; shift
    if [ "$command" != "clean" ]; then
        echo "Unexpected argument: \"$command\"." 1>&2
        exit 1
    fi
    for dir in "$temp_out" "$cpp_out" "$jni_out" "$java_out"; do
        if [ -e "$dir" ]; then
            echo "Deleting \"$dir\"..."
            rm -r "$dir"
        fi
    done
    exit
fi

# Build djinni
"$base_dir/../djinni/src/build"

[ ! -e "$temp_out" ] || rm -r "$temp_out"
"$base_dir/../djinni/src/run-assume-built" \
    --java-out "$temp_out/java" \
    --java-package $java_package \
    --java-class-access-modifier "public" \
    --java-generate-interfaces true \
    --java-use-final-for-record false \
    --java-nullable-annotation "androidx.annotation.Nullable" \
    --java-nonnull-annotation "androidx.annotation.NonNull" \
    --ident-java-field mFooBar \
    --ident-java-enum foo_bar \
    \
    --hpp-ext "h" \
    --cpp-out "$temp_out/cpp" \
    --cpp-namespace QMedia::Api \
    --ident-cpp-enum-type FooBar \
    --ident-cpp-enum foo_bar \
    --cpp-enum-hash-workaround false \
    --cpp-use-constexpr false \
    --cpp-use-enum-class true \
    --cpp-optional-template "std::experimental::optional" \
    --cpp-optional-header "\"Utils/optional.hpp\"" \
    \
    --jni-out "$temp_out/jni" \
    --ident-jni-class NativeFooBar \
    --ident-jni-file NativeFooBar \
    \
    --objc-out "$temp_out/objc" \
    --objcpp-out "$temp_out/objc" \
    --objc-type-prefix Q \
    --objc-swift-bridging-header "Q-Include-All" \
    \
    --idl "$in"

# Copy changes from "$temp_output" to final dir.

mirror() {
    local prefix="$1" ; shift
    local src="$1" ; shift
    local dest="$1" ; shift
    mkdir -p "$dest"
    rsync -r --delete --checksum --itemize-changes "$src"/ "$dest" | sed "s/^/[$prefix]/"
}

echo "Copying generated code to final directories..."
mirror "cpp" "$temp_out/cpp" "$cpp_out"
mirror "java" "$temp_out/java" "$java_out"
mirror "jni" "$temp_out/jni" "$jni_out"
mirror "objc" "$temp_out/objc" "$objc_out"

# copy djinni support-lib to mmc project
echo "Copy djinni support-lib to mmc project directories..."
djinni_support_file_out="$qmedia_project_dirctory/djinni_support_file"
djinni_support_file_src="$djinni_src_dirctory/support-lib"
mirror "support-lib" "$djinni_support_file_src" "$djinni_support_file_out"

date > "$gen_stamp"

echo "djinni completed."
