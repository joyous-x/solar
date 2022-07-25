package com.example.testjni;

/**
 * Created by jiao on 2016/8/20.
 *
 * Method 1 :
 *    A、use "javac" to generate .class file
 *       cd current-directory, then run: javac JniFooA.java
 *    B、use "javah" and .class file to generate .h file which meet the requirements
 *       cd directory 'src/main/java', then run: javah -d 'directory-jni' -classpath . com.example.testjni.JniFooA
 *       function name must looks like : Java_ + package_name + classname + function_name_in_java
 *    C、then write implementation in .c
 */
public class JniFooA {
    public static native String sayHello(String name);
}
