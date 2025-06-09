import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatButtonModule, MatCheckboxModule,MatSlideToggleModule,MatSliderModule,
  MatCardModule ,MatButtonToggleModule,MatIconModule,MatToolbarModule,MatTabsModule} from '@angular/material';
import { FormsModule } from '@angular/forms';
import { Observable } from 'rxjs';
import {
  IMqttMessage,
  MqttModule,
  IMqttServiceOptions
} from 'ngx-mqtt';
import { ServiceWorkerModule } from '@angular/service-worker';
import { environment } from '../environments/environment';
import { AppRoutingModule } from './app-routing.module';
import { WebcontrolComponent } from './webcontrol/webcontrol.component';
import { VirtuJoystickComponent } from './webcontrol/virtu-joystick/virtu-joystick.component';
import { DirectPwmsignalComponent } from './webcontrol/direct-pwmsignal/direct-pwmsignal.component';

export const MQTT_SERVICE_OPTIONS: IMqttServiceOptions = {
  hostname: '192.168.1.205',
  port: 15675,
  path: '/ws',
  username: 'fireflyweb',
  password: 'fireflyweb'
};

@NgModule({
  declarations: [
    AppComponent,
    WebcontrolComponent,
    VirtuJoystickComponent,
    DirectPwmsignalComponent
  ],
  imports: [
    BrowserModule,FormsModule,
    BrowserAnimationsModule,
    MatButtonModule, 
    MatCheckboxModule,MatSlideToggleModule,MatSliderModule,MatCardModule,
    MatButtonToggleModule,MatIconModule,MatToolbarModule,MatTabsModule,
    MqttModule.forRoot(MQTT_SERVICE_OPTIONS),
    ServiceWorkerModule.register('ngsw-worker.js', { enabled: environment.production }),
    AppRoutingModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
