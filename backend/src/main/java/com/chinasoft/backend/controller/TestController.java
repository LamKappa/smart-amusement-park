package com.chinasoft.backend.controller;

import com.chinasoft.backend.model.request.NavigationRequest;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/test")
public class TestController {

    @GetMapping("/hello")
    public String hello() {
        System.out.println("hello");
        return "hello";
    }

    @PostMapping("/testPosition")
    public String testPosition(@RequestBody NavigationRequest navigationRequest) {
        System.out.println(navigationRequest);
        return "hi";
    }

}
