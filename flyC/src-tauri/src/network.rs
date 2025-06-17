use std::net::IpAddr;
use serde::{Deserialize, Serialize};
use tauri::command;

#[derive(Debug, Serialize, Deserialize)]
pub struct NetworkInfo {
    pub interface_name: String,
    pub ip_address: String,
    pub network_base: String, // 网段基础部分，如 "192.168.1"
}

/// 获取本机IP地址信息
#[command]
pub async fn get_local_ip() -> Result<NetworkInfo, String> {
    // 获取本机IP
    let local_ip = local_ip_address::local_ip()
        .map_err(|e| format!("Failed to get local IP: {}", e))?;
    
    if let IpAddr::V4(ipv4) = local_ip {
        let ip_str = ipv4.to_string();
        
        // 获取网段基础部分（去掉最后一位）
        let ip_parts: Vec<&str> = ip_str.split('.').collect();
        if ip_parts.len() == 4 {
            let network_base = format!("{}.{}.{}", ip_parts[0], ip_parts[1], ip_parts[2]);
            
            Ok(NetworkInfo {
                interface_name: "primary".to_string(),
                ip_address: ip_str,
                network_base,
            })
        } else {
            Err("Invalid IP address format".to_string())
        }
    } else {
        Err("IPv6 not supported".to_string())
    }
} 