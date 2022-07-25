package com.example.testjunit;

import android.app.Service;
import android.test.ServiceTestCase;

/**
 * Created by jiao on 2016/6/16.
 */
public class ServiceTest extends ServiceTestCase<Service> {
    /**
     * Constructor
     *
     * @param serviceClass The type of the service under test.
     */
    public ServiceTest(Class<Service> serviceClass) {
        super(serviceClass);
    }
}
