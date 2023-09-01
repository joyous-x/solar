import java.io.*;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

final class StringFogImpl {
    private static final String CHARSET_NAME_UTF_8 = "UTF-8";

    public StringFogImpl() {
    }

    public String encrypt(String data, String key) {
        String newData;
        try {
            newData = new String(Base64.encode(xor(data.getBytes("UTF-8"), key), 2));
        } catch (UnsupportedEncodingException var5) {
            newData = new String(Base64.encode(xor(data.getBytes(), key), 2));
        }

        return newData;
    }

    public String decrypt(String data, String key) {
        String newData;
        try {
            newData = new String(xor(Base64.decode(data, 2), key), "UTF-8");
        } catch (UnsupportedEncodingException var5) {
            newData = new String(xor(Base64.decode(data, 2), key));
        }

        return newData;
    }

    public boolean overflow(String data, String key) {
        return data != null && data.length() * 4 / 3 >= 65535;
    }

    private static byte[] xor(byte[] data, String key) {
        int len = data.length;
        int lenKey = key.length();
        int i = 0;

        for(int j = 0; i < len; ++j) {
            if (j >= lenKey) {
                j = 0;
            }

            data[i] = (byte)(data[i] ^ key.charAt(j));
            ++i;
        }

        return data;
    }
}


final class StringFog {
    private static final StringFogImpl IMPL = new StringFogImpl();

    public static String encrypt(String value) {
        return IMPL.encrypt(value, "serven_scorpion").toString();
    }

    public static String decrypt(String value) {
        return IMPL.decrypt(value, "serven_scorpion");
    }
}

public final class StringFogParser {
    void travelDir(String dirPath) {
        File directory = new File(dirPath);
        if (directory.exists() && directory.isDirectory()) {
            File[] files = directory.listFiles();
            for (File file : files) {
                if (file.isFile() && file.getName().endsWith(".java")) {
                    doOnFile(file);
                } else {
                    travelDir(file.getAbsolutePath());
                }
            }
        } else {
            System.out.println("指定目录不存在或者不是一个目录");
        }
    }

    void doOnFile(File file) {
        try {
            String filePath = file.getAbsolutePath();
            String content = StringFogParser.readToString(file);

            String pattern = "StringFog.decrypt\\(\"(.*?)\"\\)";
            Pattern regex = Pattern.compile(pattern);
            Matcher matcher = regex.matcher(content);

            Map<String, String> mapper = new HashMap<>();
            while (matcher.find()) {
                String patternString = matcher.group();
                String encryptedString = matcher.group(1);
                String decryptedString = StringFog.decrypt(encryptedString);
                mapper.put(patternString, decryptedString);
            }

            if (mapper.size() == 0) {
                return ;
            }

            String replacedString = content.toString();
            Iterator iteratorMapper = mapper.entrySet().iterator();
            while (iteratorMapper.hasNext()) {
                Map.Entry entry = (Map.Entry) iteratorMapper.next();
                String key = (String) entry.getKey();
                String val = (String) entry.getValue();
                replacedString = replacedString.replace(key, "\"" + val + "\"");
                System.out.println("\t\t " + key + " -> " + val);
            }

            BufferedWriter writer = new BufferedWriter(new FileWriter(filePath));
            writer.write(replacedString);
            writer.close();

            System.out.println("ok ===> " + filePath);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static String readToString(File file) {
        Long fileLength = file.length();
        byte[] fileContent = new byte[fileLength.intValue()];
        try {
            FileInputStream in = new FileInputStream(file);
            in.read(fileContent);
            in.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new String(fileContent);//返回文件内容,默认编码
    }
}