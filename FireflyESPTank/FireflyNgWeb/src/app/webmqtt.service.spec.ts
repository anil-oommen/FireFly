import { TestBed } from '@angular/core/testing';

import { WebmqttService } from './webmqtt.service';

describe('WebmqttService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: WebmqttService = TestBed.get(WebmqttService);
    expect(service).toBeTruthy();
  });
});
