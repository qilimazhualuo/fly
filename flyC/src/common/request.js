import { invoke } from '@tauri-apps/api/core'

export const request = ({ url, method = 'get', params = {}, data = {}, headers = {}, timeout = 5000 }) => {
    return new Promise((resolve, reject) => {
        const finalUrl = url.includes('http') ? url : url
        const options = { method, params, body: data, headers }
        if (method === 'get' || method === 'GET') {
            delete options.body
        }
        invoke('http_request', { url: finalUrl, options }).then((res) => {
            resolve(res)
        }).catch((err) => {
            reject(err)
        })
    })
}
