package com.chinasoft.backend;

import com.chinasoft.backend.service.UserService;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;

@SpringBootTest
class BackendApplicationTests {

    @Autowired
    UserService userService;

    @Test
    void contextLoads() {

    }

    @Test
    void testUser() {
        System.out.println(userService.list());
    }

}
