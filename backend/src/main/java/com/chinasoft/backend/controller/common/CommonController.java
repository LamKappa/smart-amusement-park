package com.chinasoft.backend.controller.common;

import com.chinasoft.backend.common.AliOssUtil;
import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import io.swagger.annotations.ApiOperation;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.util.UUID;

/**
 * 通用接口
 *
 * @author 孟祥硕
 */
@RestController
@RequestMapping("/common")
@Slf4j
public class CommonController {

    @Autowired
    private AliOssUtil aliOssUtil;

    /**
     * 文件上传功能
     */
    @PostMapping("/upload")
    @ApiOperation("文件上传")
    public BaseResponse<String> upload(MultipartFile file) {
        // 使用UUID来重命名，防止文件名重复
        String originalFilename = file.getOriginalFilename();
        String extensionName = originalFilename.substring(originalFilename.lastIndexOf("."));
        String objectName = UUID.randomUUID().toString() + extensionName;

        String filePath = null;
        try {
            filePath = aliOssUtil.upload(file.getBytes(), objectName);
            return ResultUtils.success(filePath);
        } catch (IOException e) {
            return ResultUtils.error(ErrorCode.SYSTEM_ERROR, "文件上传失败");
        }
    }
}