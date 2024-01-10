https://github.com/alex8088/electron-vite/blob/master/src/plugins/bytecode.ts



在Windows系统中，Data Protection API (DPAPI) 提供了一种便捷的方式加密存储敏感数据，它利用了当前用户的账户信息进行加密和解密，确保只有该用户才能访问这些数据。以下是一个简单的示例，展示如何使用DPAPI对字符串进行加密和解密：

```Cpp
#include <windows.h>
#include <wincrypt.h>
#include <string>

std::wstring ProtectData(const std::wstring& plaintext)
{
    DATA_BLOB in, out;
    in.pbData = reinterpret_cast<BYTE*>(const_cast<wchar_t*>(plaintext.c_str()));
    in.cbData = (plaintext.size() + 1) * sizeof(wchar_t);

    if (!CryptProtectData(&in, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &out))
    {
        // Handle error with GetLastError()
        return L"";
    }

    std::wstring protectedData(reinterpret_cast<wchar_t*>(out.pbData), out.cbData / sizeof(wchar_t) - 1);
    LocalFree(out.pbData);
    return protectedData;
}

std::wstring UnprotectData(const std::wstring& protectedData)
{
    DATA_BLOB in, out;
    in.pbData = reinterpret_cast<BYTE*>(const_cast<wchar_t*>(protectedData.c_str()));
    in.cbData = (protectedData.size() + 1) * sizeof(wchar_t);

    if (!CryptUnprotectData(&in, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &out))
    {
        // Handle error with GetLastError()
        return L"";
    }

    std::wstring plaintext(reinterpret_cast<wchar_t*>(out.pbData), out.cbData / sizeof(wchar_t) - 1);
    LocalFree(out.pbData);
    return plaintext;
}

int main()
{
    std::wstring myString = L"This is a secret string.";
    std::wstring protectedString = ProtectData(myString);
    std::wstring decryptedString = UnprotectData(protectedString);

    // Now 'decryptedString' should be the same as 'myString'
    return 0;
}
```
这段代码首先定义了两个辅助函数：ProtectData 和 UnprotectData，分别用于加密和解密字符串。它们都使用 CryptProtectData 和 CryptUnprotectData 函数，这是Windows DPAPI的核心接口。注意，解密操作只能由加密时的同一用户在同一台计算机上执行。

请确保包含必要的头文件，并且在编译时链接Crypt32.lib库（在Visual Studio项目设置里可以添加）。






在Mac操作系统中，与Windows的Data Protection API (DPAPI) 类似的功能是Keychain服务。Keychain是一个安全存储系统，用于保存用户的密码、私钥以及其他敏感信息，并且确保这些数据在本地加密存储，只有经过授权的应用程序和用户才能访问。

以下是使用Keychain存储敏感信息的大致步骤（通常通过Cocoa或Swift进行编程实现）：

添加到Keychain： 使用SecItemAdd函数将数据添加到Keychain中，可以选择指定访问控制策略和其他属性，如密钥类型、账户名和服务标识符等。

从Keychain检索数据： 使用SecItemCopyMatching函数根据之前设置的标识来检索存储的数据。

更新或删除Keychain中的条目： 可以使用SecItemUpdate更新已存在的数据，或用SecItemDelete删除不再需要的数据。

以下是一个简单的示例（Objective-C）：

```c++
#import <Security/Security.h>

// 添加到Keychain
- (OSStatus)addToKeychain:(NSString *)password forService:(NSString *)serviceName accountName:(NSString *)account {
    NSMutableDictionary *keychainQuery = [NSMutableDictionary dictionary];
    keychainQuery[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword;
    keychainQuery[(__bridge id)kSecAttrService] = serviceName;
    keychainQuery[(__bridge id)kSecAttrAccount] = account;
    
    NSData *passwordData = [password dataUsingEncoding:NSUTF8StringEncoding];
    keychainQuery[(__bridge id)kSecValueData] = passwordData;
    
    OSStatus status = SecItemAdd((__bridge CFDictionaryRef)keychainQuery, NULL);
    if (status != errSecSuccess && status != errSecDuplicateItem) {
        NSLog(@"Error adding item to Keychain: %d", (int)status);
    }
    return status;
}

// 从Keychain检索数据
- (NSData *)retrieveFromKeychainForService:(NSString *)serviceName accountName:(NSString *)account {
    NSMutableDictionary *keychainQuery = [NSMutableDictionary dictionary];
    keychainQuery[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword;
    keychainQuery[(__bridge id)kSecAttrService] = serviceName;
    keychainQuery[(__bridge id)kSecAttrAccount] = account;
    keychainQuery[(__bridge id)kSecReturnData] = (__bridge id)kCFBooleanTrue;
    keychainQuery[(__bridge id)kSecMatchLimit] = (__bridge id)kSecMatchLimitOne;
    
    CFDataRef passwordDataRef = NULL;
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)keychainQuery, (CFTypeRef *)&passwordDataRef);
    if (status == errSecSuccess) {
        NSData *passwordData = (__bridge_transfer NSData *)passwordDataRef;
        return passwordData;
    } else {
        NSLog(@"Error retrieving from Keychain: %d", (int)status);
        return nil;
    }
}
```
以上代码展示了如何将字符串作为二进制数据添加到Keychain中以及如何检索这些数据。通过Keychain，开发人员可以利用系统的加密机制保护用户数据，而且不需要直接处理加密细节。