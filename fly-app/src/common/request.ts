import { invoke } from '@tauri-apps/api/core'

// 定义HTTP方法类型
export type HttpMethod = 'GET' | 'POST' | 'PUT' | 'DELETE' | 'PATCH' | 'HEAD' | 'OPTIONS'

// 定义请求参数接口
export interface RequestOptions {
  url: string
  method?: HttpMethod | string
  params?: Record<string, unknown>
  data?: unknown
  headers?: Record<string, string>
}

// 定义Tauri命令的选项接口
interface TauriHttpOptions {
  method: string
  params: Record<string, unknown>
  body?: unknown
  headers: Record<string, string>
}

// 定义响应类型 - 可以根据实际需要进一步细化
export type HttpResponse = unknown

export const request = ({
  url,
  method = 'GET',
  params = {},
  data = {},
  headers = {},
}: RequestOptions): Promise<HttpResponse> => {
  return new Promise((resolve, reject) => {
    const finalUrl = url.includes('http') ? url : url
    const options: TauriHttpOptions = {
      method: method.toUpperCase(),
      params,
      body: data,
      headers,
    }

    // GET请求不应该有请求体
    if (method.toUpperCase() === 'GET') {
      delete options.body
    }

    invoke('http_request', { url: finalUrl, options })
      .then((res: HttpResponse) => {
        resolve(res)
      })
      .catch((err: Error) => {
        reject(err)
      })
  })
}

// 便捷方法
export const get = (
  url: string,
  params?: Record<string, unknown>,
  headers?: Record<string, string>,
): Promise<HttpResponse> => {
  return request({ url, method: 'GET', params, headers })
}

export const post = (
  url: string,
  data?: unknown,
  headers?: Record<string, string>,
): Promise<HttpResponse> => {
  return request({ url, method: 'POST', data, headers })
}

export const put = (
  url: string,
  data?: unknown,
  headers?: Record<string, string>,
): Promise<HttpResponse> => {
  return request({ url, method: 'PUT', data, headers })
}

export const del = (url: string, headers?: Record<string, string>): Promise<HttpResponse> => {
  return request({ url, method: 'DELETE', headers })
}

export default request
