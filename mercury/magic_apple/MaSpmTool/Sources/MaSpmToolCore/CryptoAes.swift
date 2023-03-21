import Foundation

class CryptoAes {
    
    func encryptCBC(raw: String, key: String, iv: String) -> String {
        let aes = try AES(key: key, iv: iv)
        let encrypted = try aes.encrypt(raw.bytes)
        return encrypted.toBase64()
    }
    
    func decryptCBC(encrypted: String, key: String, iv: String) -> String {
        let aes = try AES(key: key, iv: iv)
        
        
       //开始解密1（从加密后的字符数组解密）
       let decrypted1 = try aes.decrypt(encrypted)
       print("解密结果1：\(String(data: Data(decrypted1), encoding: .utf8)!)")
        
       //开始解密2（从加密后的base64字符串解密）
       let decrypted2 = try encryptedBase64?.decryptBase64ToString(cipher: aes)
       print("解密结果2：\(decrypted2!)")
    }
    
    
    do {
    } catch { }
}
